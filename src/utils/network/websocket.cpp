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
	, m_port(port)
	, m_binary_data_callback(nullptr)
	, m_text_data_callback(nullptr)
	, m_connected_callback(nullptr)
{
}

WebSocket::~WebSocket() {
	for(auto* client : m_clients) {
		close(client);
		delete client;
	}
}

void WebSocket::set_binary_data_callback(std::function<void(Client* client,const void* data, size_t size)> callback) {
	m_binary_data_callback = callback;
}

void WebSocket::set_text_data_callback(std::function<void(Client* client,const std::string& data)> callback) {
	m_text_data_callback = callback;
}

void WebSocket::set_connected_callback(std::function<void(Client* client)> callback) {
	m_connected_callback = callback;
}

void WebSocket::set_http_callback(std::function<void(Client* client,const std::map<std::string, std::string>& headers, const std::string&)> callback) {
	m_http_callback = callback;
}

void WebSocket::listen() {
	m_socket.listen(m_port);
}

void WebSocket::close(Client* client) {
	if(client->established) {
		send_frame(client, 0x8, nullptr, 0);
	}
	delete client->sck;
	client->sck = nullptr;
	m_closed_clients.push_back(client);
}

void WebSocket::update() {
	// Erase closed clients
	for(auto* client : m_closed_clients) {
		for(auto it=m_clients.begin(); it != m_clients.end(); ++it) {
			if(*it == client) {
				m_clients.erase(it);
				break;
			}
		}
	}
	m_closed_clients.clear();

	for(auto* client : m_clients) {
		if(client->established) {
			ssize_t s = client->sck->read();
			if(s == -2) {
				Logging::warning(websocket_log, "Connection lost\n");
				close(client);
			} else if(s > 0) {
				const unsigned char* data = (const unsigned char*)client->sck->buffer();

				while(s > 0) {
					// Read frame
					char fin = (data[0] >> 7) & 0x1;
					char rsv = (data[0] >> 4) & 0x7;

					if(fin != 0x1) {
						Logging::error(websocket_log, "Fragmented messages are not supported\n");
						return;
					}

					if(rsv != 0) {
						Logging::error(websocket_log, "RSV must be 0!\n");
						close(client);
						return;
					}

					size_t payload_size;

					char opcode = data[0] & 0xF;
					char mask_bit = (data[1] >> 7) & 0x1;
					char payload_length = data[1] & 0x7f;

					if(mask_bit != 1) {
						// RFC requires clients to always use MASK
						Logging::error(websocket_log, "Mask bit was not set!\n");
						close(client);
						return;
					}

					int next_byte = 2;

					if(payload_length < 0) {
						Logging::error(websocket_log, "Invalid payload: %d\n", payload_length);
						close(client);
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
						close(client);
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
									m_text_data_callback(client, msg);
								} else {
									Logging::warning(websocket_log, "Got text message, but no text data callback registered (%s).\n", msg.c_str());
								}
							}
							break;
						case 0x2:
							if(m_binary_data_callback) {
								m_binary_data_callback(client, payload, payload_size);
							} else {
								Logging::warning(websocket_log, "Got binary message, but no binary data callback registered.\n");
							}
							break;
						// Control frames:
						case 0x8:
							Logging::info(websocket_log, "Client closed connection.\n");
							close(client);
							break;
						case 0x9:
							send_frame(client, 0xA, payload, static_cast<uint16_t>(payload_size));
							break;
						case 0xA:
							// pong (whatever)
							break;
					}

					delete[] payload;

					s -= (next_byte);
					s -= payload_size;

					data += (next_byte + payload_size);
				}
			}
		} else {
			// Handle handshake
			ssize_t s = client->sck->read();
			if(s == -2) {
				close(client);
			} else if(s > 0) {
				std::string msg = std::string((const char*)client->sck->buffer());
				std::vector<std::string> lines = str_split(msg, "\r\n", SPLIT_TRIM | SPLIT_IGNORE_EMPTY);
				std::map<std::string,std::string> headers;
				std::string request;

				bool is_websocket = false;
				bool key_found = false;
				std::string ws_key;
				auto it = lines.begin();
				if(it == lines.end()) {
					Logging::error(websocket_log, "WebSocket: No data.\n");
					close(client);
					return;
				}
				request = *(it++);

				for(;it!=lines.end(); ++it) {
					std::string& line = *it;
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
								close(client);
								return;
							}
						} else if(key == "Sec-WebSocket-Key") {
							ws_key = data;
							key_found = true;
						}
					}
				}
				if(!is_websocket) {
					if(m_http_callback) {
						m_http_callback(client, headers, request);
					} else {
						Logging::error(websocket_log, "Client is not using websockets.\n");
						close(client);
					}
					return;
				} else if(!key_found) {
					Logging::error(websocket_log, "Sec-WebSocket-Key not found in header.\n");
					close(client);
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

					client->established = true;
					client->sck->send(buffer, strlen(buffer));
					if(m_connected_callback) m_connected_callback(client);

				}

			}
		}
	}

	Socket* new_client = m_socket.accept();
	if(new_client) {
		m_clients.push_back(new Client(new_client));
	}
}

void WebSocket::send_raw(Client* client, const char* data, size_t size) {
	client->sck->send(data, size);
}

void WebSocket::send_binary(Client* client, const void* data, size_t size) {
	send_frame(client, 0x2, data, static_cast<uint16_t>(size));
}

void WebSocket::send_text(Client* client, const std::string& text) {
	send_frame(client, 0x1, text.c_str(), static_cast<uint16_t>(text.length()));
}

void WebSocket::send_frame(Client* client, int opcode, const void* payload, uint16_t payload_length) {
	if(client->sck && client->established) {
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

		if(!client->sck->send(msg, msg_size)) {
			Logging::error(websocket_log, "Failed to send message.\n");
		}
	} else {
		Logging::warning(websocket_log, "Trying to send data without any connection.\n");
	}
}
