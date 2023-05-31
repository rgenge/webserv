#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "HttpAns.hpp"
#include <string.h>

Server::Server() {
}

Server::Server(int port) {
	this->_socket = new Socket(port, 10);
	bzero(this->_request, sizeof(this->_request));
}

Server::~Server() {
	delete this->_socket;
}

void	Server::initialize(void) {
	try {
		_socket->initialize();
	} catch (std::exception &e) {
		throw std::runtime_error("In socket initialization");
	}
	while (1) {
		std::cout << "Waiting for connection" << std::endl;
		try {
			_requestfd = _socket->acceptConnection();

		} catch (std::exception &e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
		read(_requestfd, _request, 10000);
		std::cout << _request << std::endl;
		// aqui vai um parser da resposta lida, para responder adequadamente
		_response += HttpAns::GET_HTML;
		_getHtmlIndex();
		write(_requestfd, _response.c_str(), _response.length());
		_response.erase(0, std::string::npos);
		std::cout << "Response sent" << std::endl;
		close(_requestfd);
	}
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
