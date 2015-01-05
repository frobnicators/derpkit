#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if _WIN32
#include <windows.h>
#undef ERROR
#endif

#include <derpkit/utils/network/socket.hpp>
#include <derpkit/utils/logging.hpp>

#include "ffs_compat.hpp"

static Logging::Component socket_log = Logging::add_component("Socket");
static bool initialized = false;

#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

struct Socket::socket_t {
	socket_t() {};
	socket_t(const socket_t* socket) {
	}
};

struct Socket::address_t {
};
#else

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

struct Socket::socket_t {
	int fd;
	socket_t() {};
	socket_t(int fd) : fd(fd) {};
	socket_t(const socket_t* socket) {
		fd = socket->fd;
	}
};

struct Socket::address_t {
	sockaddr_in addr;
};
#endif

/*
 * Begin platform independant code
 */

static void platform_initialize();
static void platform_cleanup();

void Socket::initialize() {
	platform_initialize();
	initialized = true;
}

void Socket::cleanup() {
	platform_cleanup();
	initialized = false;
}

Socket::Socket(type_t type, int flags, size_t buffer_size)
	: m_flags(flags)
	, m_buffer_size(buffer_size)
	, m_buffer(nullptr)
{
	FFS_ASSERT_FATAL(initialized, "Created socket before Socket::initialize() is called.\n");
	m_hndl = new socket_t;
	m_buffer = new char[m_buffer_size];
	create_socket(type, flags);
	socket_init();
}

Socket::Socket(socket_t* socket, int flags, size_t buffer_size)
	: m_hndl(socket)
	, m_flags(flags)
	, m_buffer_size(buffer_size)
	, m_buffer(nullptr)
{
	m_buffer = new char[m_buffer_size];
	socket_init();
}

Socket::Socket(const Socket& socket)
	: m_flags(socket.m_flags)
	, m_buffer_size(socket.m_buffer_size)
	, m_buffer(nullptr)
{
	m_buffer = new char[m_buffer_size];
	m_hndl = new socket_t(socket.m_hndl);
}

Socket::~Socket() {
	close();
	delete m_hndl;
	delete[] m_buffer;
}

void Socket::set_buffer_size(size_t size) {
	delete[] m_buffer;
	m_buffer_size = size;
	m_buffer = new char[m_buffer_size];
}

Socket::Address::Address(address_t* addr) : addr(addr) {};

Socket::Address& Socket::Address::operator=(const Address& other) {
	addr = other.addr;
	return *this;
}

/*
 * End platform independant code
 */

#ifdef _WIN32

/*
 * Begin Winsock implementation
 */

static void platform_initialize() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
}

static void platform_cleanup() {
}

void Socket::socket_init() {
	//set_non_blocking(m_hndl->fd);
}

void Socket::create_socket(type_t type, int flags) {
	/*
	int one = 1;

	int stype;
	switch(type) {
		case UDP:
			stype = SOCK_DGRAM;
			break;
		case TCP:
			stype = SOCK_STREAM;
			break;
		default:
			Logging::fatal(socket_log, "Unknown socket type %d\n", (int)type);
	}
	m_hndl->fd = socket(AF_INET, stype, 0);

	if(m_hndl->fd < 0) {
		Logging::fatal(socket_log, "Failed to open socket: %s\n", strerror(errno));
	}

	if(flags & BROADCAST) {
		if(setsockopt(m_hndl->fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof(int)) == -1) {
			Logging::fatal(socket_log, "Failed to set SO_BROADCAST: %s\n", strerror(errno));
		}
	}

	if(flags & REUSE_ADDR) {
		if(setsockopt(m_hndl->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
			Logging::fatal(socket_log, "Failed to set SO_REUSEADDR: %s\n", strerror(errno));
		}
	}
	*/
}

void Socket::close() {
	//::close(m_hndl->fd);
}

static void create_addr(sockaddr_in* addr, uint32_t address, int port) {
	/*
	memset((char *) addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(static_cast<uint16_t>(port));
	addr->sin_addr.s_addr = htonl(address);
	*/
}

std::string Socket::Address::hostname() const {
	/*
	if(addr == nullptr) return "<nullptr>";
	char ipstr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr->addr.sin_addr, ipstr, sizeof ipstr);
	return std::string(ipstr);
	*/
	return "";
}

int Socket::Address::port() const {
	/*
	if(addr == nullptr) return -1;
	return ntohs(addr->addr.sin_port);
	*/
	return 0;
}

Socket::Address Socket::get_peer() const {
/*	socklen_t len;
	struct sockaddr_storage addr;

	len = sizeof addr;
	getpeername(m_hndl->fd, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		 struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		 address_t* address = new address_t;
		 memcpy(&address->addr, s, sizeof(sockaddr_in));
		 return Address(address);

	} else { // AF_INET6
		Logging::error(socket_log, "get_peer not implemented for ipv6.\n");
		return Address();
	}*/
	return Address();
}

Socket::Address Socket::create_address(const char* hostname, int port) {
	/*hostent * host;
	host = gethostbyname(hostname);
	if(host == NULL) {
		Logging::error("No such host %s\n", hostname);
		return Address();
	}

	address_t* addr = new address_t;

	create_addr(&addr->addr,0,port);

	memcpy(
		(char *)&addr->addr.sin_addr.s_addr,
		(char *)host->h_addr,
		host->h_length
	);

	return Address(addr);*/
	return Address();
}

Socket::Address Socket::create_broadcast_address(int port) {
	/*
	address_t* addr = new address_t;
	create_addr(&addr->addr, INADDR_BROADCAST,port);
	return Address(addr);
	*/
	return Address();
}

Socket::Address Socket::create_any_address(int port) {
/*	address_t* addr = new address_t;
	create_addr(&addr->addr, INADDR_ANY,port);
	return Address(addr);*/
	return Address();
}

bool Socket::connect(const char* hostname, int port) {
	//return connect(create_address(hostname, port));
	return false;
}

bool Socket::connect(const Address& address) {
	/*
	if(address.addr == nullptr) {
		Logging::error("Address is null.\n");
		return false;
	}

	// We want connect to be blocking
	set_blocking(m_hndl->fd);

	if (::connect(m_hndl->fd,(sockaddr *) &address.addr->addr,sizeof(address.addr->addr)) < 0)  {
		Logging::error(socket_log, "Failed to connect to %s:%d: %s\n",
				address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	set_non_blocking(m_hndl->fd);

	Logging::verbose(socket_log, "Connected to %s:%d\n", address.hostname().c_str(), address.port());

	return true;
	*/
	return false;
}

bool Socket::bind(int port) {
	/*if (m_flags & BROADCAST) {
		return bind(create_broadcast_address(port));
	} else {
		return bind(create_any_address(port));
	}*/
	return false;
}

bool Socket::bind(const Address& address) {
	/*
	if(::bind(m_hndl->fd, (sockaddr *) &address.addr->addr, sizeof(sockaddr_in)) < 0) {
		Logging::error(socket_log, "Failed to bind to %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	Logging::verbose(socket_log, "Bound to %s:%d\n", address.hostname().c_str(), address.port());
	return true;
	*/
	return false;
}

bool Socket::listen(int port, int queue_size) {
	//return listen(create_any_address(port), queue_size);
	return false;
}

bool Socket::listen(const Address& address, int queue_size) {
	/*
	if (::bind(m_hndl->fd, (sockaddr *) &address.addr->addr, sizeof(sockaddr_in)) < 0)  {
		Logging::error(socket_log, "Failed to bind to %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	if(::listen(m_hndl->fd,queue_size) < 0) {
		Logging::error(socket_log, "Failed to listen on %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	Logging::verbose(socket_log, "Listening on %s:%d\n", address.hostname().c_str(), address.port());

	return true;
	*/
	return false;
}

Socket* Socket::accept() {
	/*
	sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = ::accept(m_hndl->fd, (sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		return nullptr;

	return new Socket(new socket_t(newsockfd), 0, m_buffer_size);
	*/
	return nullptr;
}

bool Socket::send(const void* data, size_t size, const Address& address) {
	/*sockaddr* dest_addr = nullptr;
	socklen_t addrlen = 0;
	if(address.addr != nullptr) {
		dest_addr = (sockaddr*) &address.addr->addr;
		addrlen = sizeof(address.addr->addr);
	}
	if(::sendto(m_hndl->fd, data, size, MSG_NOSIGNAL, dest_addr, addrlen) < 0) {
		Logging::error(socket_log, "Failed to send data: %s\n", strerror(errno));
		return false;
	}
	return true;
	*/
	return false;
}

ssize_t Socket::read(const Address& address, size_t limit) {
	/*if(limit == 0) limit = m_buffer_size;
	FFS_ASSERT_FATAL(limit <= m_buffer_size, "read(): Limit (%lu) can't be larger than buffer size (%lu)\n", limit, m_buffer_size);

	sockaddr* src_addr = nullptr;
	socklen_t src_len = 0;
	if(address.addr != nullptr) {
		src_addr = (sockaddr*) &address.addr->addr;
		src_len = sizeof(address.addr->addr);
	}

	ssize_t size = recvfrom(m_hndl->fd,m_buffer,limit, 0, src_addr, &src_len);
	if(size == -1) {
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data available
			return 0;
		} else {
			Logging::error(socket_log, "Error when reading from socket: %s\n",strerror(errno));
			return -1;
		}
	} else if(size == 0) {
		Logging::warning(socket_log, "When reading: Socket is closed.\n");
		return -2;
	} else {
		return size;
	}*/
	return - 1;
}

uint32_t network::host_to_network_order(uint32_t hostlong) {
	return htonl(hostlong);
}

uint16_t network::host_to_network_order(uint16_t hostshort) {
	return htons(hostshort);
}

uint32_t network::network_to_host_order(uint32_t netlong) {
	return ntohl(netlong);
}

uint16_t network::network_to_host_order(uint16_t netshort) {
	return ntohs(netshort);
}
#else

/*
 * Begin BSD socket implementation
 */

static void platform_initialize() {
}

static void platform_cleanup() {
}

void static set_non_blocking(int fd) {
	int x;
	x=fcntl(fd,F_GETFL,0);
	fcntl(fd,F_SETFL,x | O_NONBLOCK);
}

void static set_blocking(int fd) {
	int x;
	x=fcntl(fd,F_GETFL,0);
	fcntl(fd,F_SETFL,x & ~O_NONBLOCK);
}

void Socket::socket_init() {
	set_non_blocking(m_hndl->fd);
}

void Socket::create_socket(type_t type, int flags) {
	int one = 1;

	int stype;
	switch(type) {
		case UDP:
			stype = SOCK_DGRAM;
			break;
		case TCP:
			stype = SOCK_STREAM;
			break;
		default:
			Logging::fatal(socket_log, "Unknown socket type %d\n", (int)type);
	}
	m_hndl->fd = socket(AF_INET, stype, 0);

	if(m_hndl->fd < 0) {
		Logging::fatal(socket_log, "Failed to open socket: %s\n", strerror(errno));
	}

	if(flags & BROADCAST) {
		if(setsockopt(m_hndl->fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof(int)) == -1) {
			Logging::fatal(socket_log, "Failed to set SO_BROADCAST: %s\n", strerror(errno));
		}
	}

	if(flags & REUSE_ADDR) {
		if(setsockopt(m_hndl->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
			Logging::fatal(socket_log, "Failed to set SO_REUSEADDR: %s\n", strerror(errno));
		}
	}
}

void Socket::close() {
	::close(m_hndl->fd);
}

static void create_addr(sockaddr_in* addr, uint32_t address, int port) {
	memset((char *) addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(static_cast<uint16_t>(port));
	addr->sin_addr.s_addr = htonl(address);
}

std::string Socket::Address::hostname() const {
	if(addr == nullptr) return "<nullptr>";
	char ipstr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr->addr.sin_addr, ipstr, sizeof ipstr);
	return std::string(ipstr);
}

int Socket::Address::port() const {
	if(addr == nullptr) return -1;
	return ntohs(addr->addr.sin_port);
}

Socket::Address Socket::get_peer() const {
	socklen_t len;
	struct sockaddr_storage addr;

	len = sizeof addr;
	getpeername(m_hndl->fd, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		 struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		 address_t* address = new address_t;
		 memcpy(&address->addr, s, sizeof(sockaddr_in));
		 return Address(address);

	} else { // AF_INET6
		Logging::error(socket_log, "get_peer not implemented for ipv6.\n");
		return Address();
		/*
		 char ipstr[INET6_ADDRSTRLEN];
		   struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		   inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	   */
	}
}

Socket::Address Socket::create_address(const char* hostname, int port) {
	hostent * host;
	host = gethostbyname(hostname);
	if(host == NULL) {
		Logging::error("No such host %s\n", hostname);
		return Address();
	}

	address_t* addr = new address_t;

	create_addr(&addr->addr,0,port);

	memcpy(
		(char *)&addr->addr.sin_addr.s_addr,
		(char *)host->h_addr,
		host->h_length
	);

	return Address(addr);
}

Socket::Address Socket::create_broadcast_address(int port) {
	address_t* addr = new address_t;
	create_addr(&addr->addr, INADDR_BROADCAST,port);
	return Address(addr);
}

Socket::Address Socket::create_any_address(int port) {
	address_t* addr = new address_t;
	create_addr(&addr->addr, INADDR_ANY,port);
	return Address(addr);
}

bool Socket::connect(const char* hostname, int port) {
	return connect(create_address(hostname, port));
}

bool Socket::connect(const Address& address) {
	if(address.addr == nullptr) {
		Logging::error("Address is null.\n");
		return false;
	}

	// We want connect to be blocking
	set_blocking(m_hndl->fd);

	if (::connect(m_hndl->fd,(sockaddr *) &address.addr->addr,sizeof(address.addr->addr)) < 0)  {
		Logging::error(socket_log, "Failed to connect to %s:%d: %s\n",
				address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	set_non_blocking(m_hndl->fd);

	Logging::verbose(socket_log, "Connected to %s:%d\n", address.hostname().c_str(), address.port());

	return true;
}

bool Socket::bind(int port) {
	if (m_flags & BROADCAST) {
		return bind(create_broadcast_address(port));
	} else {
		return bind(create_any_address(port));
	}
}

bool Socket::bind(const Address& address) {

	if(::bind(m_hndl->fd, (sockaddr *) &address.addr->addr, sizeof(sockaddr_in)) < 0) {
		Logging::error(socket_log, "Failed to bind to %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	Logging::verbose(socket_log, "Bound to %s:%d\n", address.hostname().c_str(), address.port());
	return true;
}

bool Socket::listen(int port, int queue_size) {
	return listen(create_any_address(port), queue_size);
}

bool Socket::listen(const Address& address, int queue_size) {
	if (::bind(m_hndl->fd, (sockaddr *) &address.addr->addr, sizeof(sockaddr_in)) < 0)  {
		Logging::error(socket_log, "Failed to bind to %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	if(::listen(m_hndl->fd,queue_size) < 0) {
		Logging::error(socket_log, "Failed to listen on %s:%d: %s\n", address.hostname().c_str(), address.port(), strerror(errno));
		return false;
	}

	Logging::verbose(socket_log, "Listening on %s:%d\n", address.hostname().c_str(), address.port());

	return true;
}

Socket* Socket::accept() {
	sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd = ::accept(m_hndl->fd, (sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		return nullptr;

	return new Socket(new socket_t(newsockfd), 0, m_buffer_size);
}

bool Socket::send(const void* data, size_t size, const Address& address) {
	sockaddr* dest_addr = nullptr;
	socklen_t addrlen = 0;
	if(address.addr != nullptr) {
		dest_addr = (sockaddr*) &address.addr->addr;
		addrlen = sizeof(address.addr->addr);
	}
	if(::sendto(m_hndl->fd, data, size, MSG_NOSIGNAL, dest_addr, addrlen) < 0) {
		Logging::error(socket_log, "Failed to send data: %s\n", strerror(errno));
		return false;
	}
	return true;
}

ssize_t Socket::read(const Address& address, size_t limit) {
	/*
	fd_set readset;
	struct timeval tv = {0, 0};

	FD_ZERO(&readset);
	FD_SET(m_hndl->fd,&readset);

	int sval = select(m_hndl->fd+1,&readset,NULL,NULL,&tv);
	if(sval == 0) {
		return 0;
	} else if(sval < 0) {
		Logging::error(socket_log, "In read(): select returned error: %s\n", strerror(errno));
		return -1;
	}
	*/

	if(limit == 0) limit = m_buffer_size;
	FFS_ASSERT_FATAL(limit <= m_buffer_size, "read(): Limit (%lu) can't be larger than buffer size (%lu)\n", limit, m_buffer_size);

	sockaddr* src_addr = nullptr;
	socklen_t src_len = 0;
	if(address.addr != nullptr) {
		src_addr = (sockaddr*) &address.addr->addr;
		src_len = sizeof(address.addr->addr);
	}

	ssize_t size = recvfrom(m_hndl->fd,m_buffer,limit, 0, src_addr, &src_len);
	if(size == -1) {
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data available
			return 0;
		} else {
			Logging::error(socket_log, "Error when reading from socket: %s\n",strerror(errno));
			return -1;
		}
	} else if(size == 0) {
		Logging::warning(socket_log, "When reading: Socket is closed.\n");
		return -2;
	} else {
		return size;
	}
}

uint32_t network::host_to_network_order(uint32_t hostlong) {
	return htonl(hostlong);
}

uint16_t network::host_to_network_order(uint16_t hostshort) {
	return htons(hostshort);
}

uint32_t network::network_to_host_order(uint32_t netlong) {
	return ntohl(netlong);
}

uint16_t network::network_to_host_order(uint16_t netshort) {
	return ntohs(netshort);
}

/*
 * End BSD socket implementation
 */
#endif
