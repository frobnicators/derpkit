#ifndef FROB_SOCKET_HPP
#define FROB_SOCKET_HPP

#include <memory>
#include <cstdint>
#include <string>

namespace network {
	uint32_t host_to_network_order(uint32_t hostlong);
	uint16_t host_to_network_order(uint16_t hostshort);

	uint32_t network_to_host_order(uint32_t netlong);
	uint16_t network_to_host_order(uint16_t netshort);
};

class Socket {
	struct address_t;
	public:
		static void initialize();
		static void cleanup();

		enum type_t {
			UDP,
			TCP
		};

		enum flags : int {
			REUSE_ADDR = (1 << 0),
			BROADCAST  = (1 << 1), /* Only UDP */
		};

		Socket(type_t type, int flags = 0, size_t buffer_size=512);
		Socket(const Socket& socket);
		~Socket();

		/*
		 * Address container class
		 */
		class Address {
		public:
			Address() : addr(nullptr) { }
			Address& operator=(const Address& other);

			// These methods query the underlying address, so they might be expensive
			std::string hostname() const;
			int port() const;
		private:
			std::shared_ptr<address_t> addr;
			Address(address_t* addr);
			friend class Socket;
		};

		static Address create_address(const char* hostname, int port);
		static Address create_broadcast_address(int port);
		static Address create_any_address(int port);

		bool connect(const Address& addr);
		bool connect(const char* hostname, int port);

		/**
		 * Bind to address and port.
		 * Address is any or broadcast, depending on socket flags.
		 */
		bool bind(int port);

		bool bind(const Address& address);

		/**
		 * For TCP servers (also performs bind)
		 * (non blocking, call accept to check for clients)
		 */
		bool listen(int port, int queue_size=5);

		bool listen(const Address& address, int queue_size=5);

		/**
		 * Checks for new clients (when listening).
		 * @return The new client if any, otherwise nullptr
		 */
		Socket* accept();

		/**
		 * Send data, Address must be specified if connect has not been used
		 */
		bool send(const void* data, size_t size, const Address& address = Address());

		/**
		 * Reads data if any available.
		 * @param limit Read max this number of bytes (must be less or equal to buffer_size).
		 * @return number of bytes read, The data is stored in buffer(). If no data was available 0 is returned.
		 *								 On error -1 is returned.
		 *								 On closed socket -2 is returned.
		 */
		ssize_t read(const Address& address=Address(), size_t limit=0);

		inline const void* buffer() const {
			return static_cast<const void*>(m_buffer);
		}

		inline size_t buffer_size() const {
			return m_buffer_size;
		}

		void set_buffer_size(size_t size);

		void close();

		Address get_peer() const;
		bool is_broadcast() { return 0 != (m_flags & BROADCAST); }
	private:
		struct socket_t;
		socket_t* m_hndl;
		const int m_flags;
		size_t m_buffer_size;
		char* m_buffer;

		void create_socket(type_t type, int flags);
		void socket_init();

		Socket(socket_t* socket, int flags, size_t buffer_size);
};

#endif
