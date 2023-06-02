#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "HttpAns.hpp"
#include <string.h>

Server::Server() : Socket() {
	memset(this->_request, 0, sizeof(this->_request));
}

Server::Server(int port) : Socket(port, 10) {
	memset(this->_request, 0, sizeof(this->_request));
}

Server::~Server() {
}

Server::Server(Server const &rhs) : Socket(rhs._port, 10) {
	*this = rhs;
}

Server&	Server::operator=(Server const &rhs) {
	if (this == &rhs)
		return (*this);
	Socket::operator=(rhs);
	this->_response = rhs._response;
	return (*this);
}

void	Server::_getHtmlIndex(void) {
	std::ifstream	index;
	std::string		buffer;
	std::string		line;

	index.open("/home/pedro/Desktop/42/projects/webserv/index/index.html", std::ifstream::in);
	while (std::getline(index, line)) {
		buffer += line + "\n";
	}
	_response += buffer;
	index.close();
}

void	Server::handleRequest(int requestfd) {
	// this->_requestfds.push_back(fd);
	std::cout << "Request accepted!\n" << std::endl;
	read(requestfd, _request, 10000);
	std::cout << _request << std::endl;
	// aqui vai um parser da resposta lida, para responder adequadamente
	_response += HttpAns::GET_HTML;
	_getHtmlIndex();
	write(requestfd, _response.c_str(), _response.length());
	_response.erase(0, std::string::npos);
	std::cout << "Response sent\n" << std::endl;
	close(requestfd);
}
