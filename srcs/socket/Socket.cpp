#include "Socket.hpp"
#include <iostream>
#include <exception>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket() {
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(0);
	memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));
	this->_addrlen = sizeof(this->_address);
	this->_socketaddr = reinterpret_cast<struct sockaddr *>(&this->_address);
	this->_socketAddrlen = reinterpret_cast<socklen_t *>(&this->_addrlen);
	this->_socketfd = -1;
}

Socket::Socket(int port, int backlog) : _port(port), _backlog(backlog) {
	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(this->_port);
	memset(this->_address.sin_zero, '\0', sizeof(this->_address.sin_zero));
	this->_addrlen = sizeof(this->_address);
	this->_socketaddr = reinterpret_cast<struct sockaddr *>(&this->_address);
	this->_socketAddrlen = reinterpret_cast<socklen_t *>(&this->_addrlen);
	this->_socketfd = -1;
}

Socket::Socket(Socket const &rhs) {
	*this = rhs;
}

Socket&	Socket::operator=(Socket const &rhs) {
	if (&rhs == this)
		return (*this);
	this->_address = rhs._address;
	this->_addrlen = rhs._addrlen;
	*this->_socketAddrlen = *rhs._socketAddrlen;
	*this->_socketaddr = *rhs._socketaddr;
	this->_backlog = rhs._backlog;
	this->_port = rhs._port;
	this->_socketfd = rhs._socketfd;
	return (*this);
}

Socket::~Socket(void) {
}

void	Socket::initialize(void) {
	if ((this->_socketfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // Estes parametros serão provavelmente variáveisz
		throw std::runtime_error("When creating socket");
	if (this->_bindSocket() < 0)
		throw std::runtime_error("When binding socket");
	if (_listen(this->_backlog) < 0)
		throw std::runtime_error("When listening socket");
}

int	Socket::_bindSocket(void) {
	return (bind(this->_socketfd, this->_socketaddr, this->_addrlen));
}

int	Socket::_listen(int backlog) {
	return (listen(this->_socketfd, backlog));
}

int	Socket::acceptConnection(void) {
	int				newSocket;

	if ((newSocket = accept(this->_socketfd, this->_socketaddr, this->_socketAddrlen)) < 0)
		throw std::runtime_error("In accepting connection");
	return (newSocket);
}

void	Socket::closeSocket(void) const {
	close(this->_socketfd);
}

int	Socket::getSocketFd(void) const {
	return (this->_socketfd);
}
