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

		void set_binary_data_callback(std::function<void(const void* data, size_t size)> callback);
		void set_text_data_callback(std::function<void(const std::string& data)> callback);
		void set_connected_callback(std::function<void(void)> callback);

		/**
		 * Called each time the client uses normal http, and doesn't upgrade.
		 * Default behaviour: Close.
		 */
		void set_http_callback(std::function<void(const std::map<std::string, std::string>& headers, const std::vector<std::string>& data)> callback);

		void listen();
		bool connected() const { return m_established; }

		void update();
		void send_binary(const void* data, size_t size);
		void send_text(const std::string& text);

		/**
		 * Send raw data on the socket (bypass websocket protocol)
		 */
		void send_raw(const char* data, size_t size);

		void close();
	private:
		Socket m_socket;
		Socket* m_client;
		bool m_established;
		const int m_port;

		std::function<void(const void* data, size_t size)> m_binary_data_callback;
		std::function<void(const std::string& data)> m_text_data_callback;
		std::function<void(void)> m_connected_callback;
		std::function<void(const std::map<std::string, std::string>&, const std::vector<std::string>& )> m_http_callback;

		void send_frame(int opcode, const void* payload, uint16_t payload_length);
};

#endif
