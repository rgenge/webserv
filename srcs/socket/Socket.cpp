#include "Socket.hpp"
#include <iostream>
#include <exception>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

Socket::Socket(int backlog, std::string const &port) : _backlog(backlog), _port(port) {
	_optval = 1;
	memset(&_hints, 0, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, _port.c_str(), &_hints, &_serverInfo) != 0)
		throw std::runtime_error("Failed to get socket info (getaddrinfo)");
	_addrSize = sizeof(this->_clientAddr);
	this->_socketfd = -1;
}

Socket::Socket(Socket const &rhs) {
	*this = rhs;
}

Socket&	Socket::operator=(Socket const &rhs) {
	if (&rhs == this)
		return (*this);
	this->_backlog = rhs._backlog;
	this->_port = rhs._port;
	this->_socketfd = rhs._socketfd;
	this->_hints = rhs._hints;
	this->_addrSize = rhs._addrSize;
	this->_clientAddr = rhs._clientAddr;
	this->_optval = rhs._optval;
	return (*this);
}

Socket::~Socket(void) {
	freeaddrinfo(_serverInfo);
}

void	Socket::initialize(void) {
	if ((this->_socketfd = socket(_serverInfo->ai_family, _serverInfo->ai_socktype, _serverInfo->ai_protocol)) == 0)
		throw std::runtime_error("When creating socket");
	if (setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &_optval, sizeof(_optval)) == -1)
		throw std::runtime_error("Fail to set socket options (setsockopt)");
	if (this->_bindSocket() < 0)
		throw std::runtime_error("When binding socket");
	if (_listen(this->_backlog) < 0)
		throw std::runtime_error("When listening socket");
}

int	Socket::_bindSocket(void) {
	return (bind(this->_socketfd, _serverInfo->ai_addr, _serverInfo->ai_addrlen));
}

int	Socket::_listen(int backlog) {
	return (listen(this->_socketfd, backlog));

}

int	Socket::acceptConnection(void) {
	int				newSocket;

	if ((newSocket = accept(this->_socketfd, (struct sockaddr *) &this->_clientAddr, &_addrSize)) < 0)
		throw std::runtime_error("In accepting connection");
	return (newSocket);
}

void	Socket::closeSocket(void) const {
	close(this->_socketfd);
}

int	Socket::getSocketFd(void) const {
	return (this->_socketfd);
}
