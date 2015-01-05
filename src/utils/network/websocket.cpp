#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <derpkit/utils/network/websocket.hpp>
#include <derpkit/utils/string_utils.hpp>
#include <derpkit/utils/base64.hpp>

#include "ffs_compat.hpp"


#include <cstring>
#include <sha1.h>

static Logging::Component websocket_log = Logging::add_component("WebSocket");

static const char* magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WebSocket::WebSocket(int port)
	: m_socket(Socket::TCP, Socket::REUSE_ADDR, 1024)
	, m_client(nullptr)
	, m_established(false)
	, m_port(port)
	, m_binary_data_callback(nullptr)
	, m_text_data_callback(nullptr)
	, m_connected_callback(nullptr)
{
}

WebSocket::~WebSocket() {
	close();
	delete m_client;
}

void WebSocket::set_binary_data_callback(std::function<void(const void* data, size_t size)> callback) {
	m_binary_data_callback = callback;
}

void WebSocket::set_text_data_callback(std::function<void(const std::string& data)> callback) {
	m_text_data_callback = callback;
}

void WebSocket::set_connected_callback(std::function<void(void)> callback) {
	m_connected_callback = callback;
}

void WebSocket::set_http_callback(std::function<void(const std::map<std::string, std::string>& headers, const std::vector<std::string>&)> callback) {
	m_http_callback = callback;
}

void WebSocket::listen() {
	m_socket.listen(m_port);
}

void WebSocket::close() {
	if(m_client && m_established) {
		send_frame(0x8, nullptr, 0);
	}
	delete m_client;
	m_client = nullptr;
	m_established = false;
}

void WebSocket::update() {
	if(m_client) {
		if(m_established) {
			ssize_t s = m_client->read();
			if(s == -2) {
				Logging::warning(websocket_log, "Connection lost\n");
				close();
			} else if(s > 0) {
				const unsigned char* data = (const unsigned char*)m_client->buffer();

				// Read frame
				char fin = (data[0] >> 7) & 0x1;
				char rsv = (data[0] >> 4) & 0x7;

				if(fin != 0x1) {
					Logging::error(websocket_log, "Fragmented messages are not supported\n");
					return;
				}

				if(rsv != 0) {
					Logging::error(websocket_log, "RSV must be 0!\n");
					close();
					return;
				}

				size_t payload_size;

				char opcode = data[0] & 0xF;
				char mask_bit = (data[1] >> 7) & 0x1;
				char payload_length = data[1] & 0x7f;

				if(mask_bit != 1) {
					// RFC requires clients to always use MASK
					Logging::error(websocket_log, "Mask bit was not set!\n");
					close();
					return;
				}

				int next_byte = 2;

				if(payload_length < 0) {
					Logging::error(websocket_log, "Invalid payload: %d\n", payload_length);
					close();
					return;
				} else if(payload_length < 126) {
					payload_size = payload_length;
				} else if(payload_length == 126) {
					uint16_t pl;
					char* p = (char*)&pl;
					p[0] = data[next_byte++];
					p[1] = data[next_byte++];
					payload_size = network::network_to_host_order(pl);
				} else if(payload_length == 127) {
					// 64 bit payload size? Madness!
					Logging::error(websocket_log, "64 bit payload length not supported\n");
					close();
					return;
				}

				uint32_t mask;
				char* mask_ptr = (char*)&mask;
				mask_ptr[0] = data[next_byte++];
				mask_ptr[1] = data[next_byte++];
				mask_ptr[2] = data[next_byte++];
				mask_ptr[3] = data[next_byte++];

				char* payload = new char[payload_size];
				memcpy(payload, data + next_byte, payload_size);

				// Unmask
				for(size_t i=0; i<payload_size; ++i) {
					payload[i] ^= mask_ptr[i % 4];
				}

				switch(opcode) {
					case 0x0:
						Logging::error(websocket_log, "Continuation frames not supported\n");
						return;
					case 0x1:
						{
							// text
							std::string msg(payload, payload_size);
							if(m_text_data_callback) {
								m_text_data_callback(msg);
							} else {
								Logging::warning(websocket_log, "Got text message, but no text data callback registered (%s).\n", msg.c_str());
							}
						}
						break;
					case 0x2:
						if(m_binary_data_callback) {
							m_binary_data_callback(payload, payload_size);
						} else {
							Logging::warning(websocket_log, "Got binary message, but no binary data callback registered.\n");
						}
						break;
					// Control frames:
					case 0x8:
						Logging::info(websocket_log, "Client closed connection.\n");
						close();
						break;
					case 0x9:
						send_frame(0xA, payload, static_cast<uint16_t>(payload_size));
						break;
					case 0xA:
						// pong (whatever)
						break;
				}

				delete[] payload;
			}
		} else {
			// Handle handshake
			ssize_t s = m_client->read();
			if(s == -2) {
				close();
			} else if(s > 0) {
				std::string msg = std::string((const char*)m_client->buffer());
				std::vector<std::string> lines = str_split(msg, "\r\n", SPLIT_TRIM | SPLIT_IGNORE_EMPTY);
				std::map<std::string,std::string> headers;
				std::vector<std::string> data;

				bool is_websocket = false;
				bool key_found = false;
				std::string ws_key;
				for(const std::string& line : lines) {
					std::vector<std::string> key_data = str_split(line, ":", SPLIT_TRIM, 1);
					if(key_data.size() == 2) {
						std::string key = key_data[0];
						std::string data = key_data[1];
						headers[key] = data;
						if(key == "Upgrade") {
							if(data == "websocket") {
								is_websocket = true;
							} else {
								Logging::error(websocket_log, "Incorrect upgrade %s, (must be 'websocket')\n", data.c_str());
								close();
								return;
							}
						} else if(key == "Sec-WebSocket-Key") {
							ws_key = data;
							key_found = true;
						}
					} else {
						data.push_back(line);
					}
				}
				if(!is_websocket) {
					if(m_http_callback) {
						m_http_callback(headers, data);
					} else {
						Logging::error(websocket_log, "Client is not using websockets.\n");
						close();
					}
					return;
				} else if(!key_found) {
					Logging::error(websocket_log, "Sec-WebSocket-Key not found in header.\n");
					close();
					return;
				}

				{
					std::string accept_key = ws_key + magic_string;
					char hash[20];
					sha1::calc(accept_key.c_str(), static_cast<int>(accept_key.length()), (unsigned char*)hash);

					std::string b64 = base64::encode(hash, 20);

					char buffer[256];

					sprintf(buffer, "HTTP/1.1 101 Switching Protocols\r\n"
						"Upgrade: websocket\r\n"
						"Connection: Upgrade\r\n"
						"Sec-WebSocket-Accept: %s\r\n"
						"\r\n"
						, b64.c_str());

					m_established = true;
					m_client->send(buffer, strlen(buffer));
					if(m_connected_callback) m_connected_callback();

				}

			}
		}
	} else {
		m_client = m_socket.accept();
		m_established = false;
		if(m_client) update();
	}
}

void WebSocket::send_raw(const char* data, size_t size) {
	m_client->send(data, size);
}

void WebSocket::send_binary(const void* data, size_t size) {
	send_frame(0x2, data, static_cast<uint16_t>(size));
}

void WebSocket::send_text(const std::string& text) {
	send_frame(0x1, text.c_str(), static_cast<uint16_t>(text.length()));
}

void WebSocket::send_frame(int opcode, const void* payload, uint16_t payload_length) {
	if(m_client && m_established) {
		size_t msg_size = 2 + payload_length;
		char pl;

		if(payload_length > 125) {
			pl = 126;
			msg_size += 2;
		} else {
			pl = (char)payload_length;
		}

		char* msg = new char[msg_size];
		memset(msg, 0, msg_size);
		msg[0] = static_cast<char>((0x80) | (opcode & 0xF));
		msg[1] = pl & 0x7F;


		int next_byte = 2;
		if(payload_length > 125) {
			uint16_t network_pl = network::host_to_network_order(payload_length);
			char* p = (char*)&network_pl;
			msg[next_byte++] = p[0];
			msg[next_byte++] = p[1];
		}

		memcpy(msg + next_byte, payload, payload_length);

		if(!m_client->send(msg, msg_size)) {
			Logging::error(websocket_log, "Failed to send message.\n");
		}
	} else {
		Logging::warning(websocket_log, "Trying to send data without any connection.\n");
	}
}
