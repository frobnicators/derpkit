#ifndef FROB_WEBSOCKET_HPP
#define FROB_WEBSOCKET_HPP

#include <derpkit/utils/network/socket.hpp>
#include <functional>
#include <string>
#include <map>
#include <vector>

/**
 * A simple implementation of a websocket server
 * TODO: Support multiple clients
 */
class WebSocket {
	public:
		WebSocket(int port);
		~WebSocket();

		struct Client {
			Socket* sck;
			bool established;
			Client(const Client&) = delete;

			bool connected() const { return established; }

			Client(Socket* sck) : sck(sck), established(false) {}
			~Client() {
				delete sck;
			}
		};

		void set_binary_data_callback(std::function<void(Client* client, const void* data, size_t size)> callback);
		void set_text_data_callback(std::function<void(Client* client, const std::string& data)> callback);
		void set_connected_callback(std::function<void(Client* client)> callback);

		/**
		 * Called each time the client uses normal http, and doesn't upgrade.
		 * Default behaviour: Close.
		 */
		void set_http_callback(std::function<void(Client* client, const std::map<std::string, std::string>& headers, const std::string& request)> callback);

		void listen();

		void close(Client* client);

		void update();
		void send_binary(Client* client, const void* data, size_t size);
		void send_text(Client* client, const std::string& text);

		/**
		 * Send raw data on the socket (bypass websocket protocol)
		 */
		void send_raw(Client* client, const char* data, size_t size);
	private:
		Socket m_socket;

		std::vector<Client*> m_clients;
		std::vector<Client*> m_closed_clients;
		const int m_port;

		std::function<void(Client* client,const void* data, size_t size)> m_binary_data_callback;
		std::function<void(Client* client,const std::string& data)> m_text_data_callback;
		std::function<void(Client* client)> m_connected_callback;
		std::function<void(Client* client,const std::map<std::string, std::string>&, const std::string& )> m_http_callback;

		void send_frame(Client* client, int opcode, const void* payload, uint16_t payload_length);
};

#endif
