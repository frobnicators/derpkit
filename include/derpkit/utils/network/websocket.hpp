#ifndef FROB_WEBSOCKET_HPP
#define FROB_WEBSOCKET_HPP

#include <derpkit/utils/network/socket.hpp>
#include <functional>

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

		void listen();
		bool connected() const { return m_established; }

		void update();
		void send_binary(const void* data, size_t size);
		void send_text(const std::string& text);

		void close();
	private:
		Socket m_socket;
		Socket* m_client;
		bool m_established;
		const int m_port;

		std::function<void(const void* data, size_t size)> m_binary_data_callback;
		std::function<void(const std::string& data)> m_text_data_callback;
		std::function<void(void)> m_connected_callback;

		void send_frame(int opcode, const void* payload, uint16_t payload_length);
};

#endif
