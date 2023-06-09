#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "HttpAns.hpp"
#include "Request.hpp"
#include <string.h>

Server::Server() : Socket() {
}

Server::Server(int port) : Socket(port, 10) {
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
	return (*this);
}

std::string	Server::_getHtmlIndex(void) {
	std::ifstream	index;
	std::string		buffer;
	std::string		line;

	index.open("./index/index.html", std::ifstream::in);
	while (std::getline(index, line)) {
		buffer += line + "\n";
	}
	index.close();
	std::cout << "buffer size: " << buffer.size() << std::endl;
	return (buffer);
}

void	Server::addRequestfd(int requestfd, std::string requestMessage) {
	_requestfds.insert(std::pair<int, std::string>(requestfd, requestMessage));
}

int	Server::getRequest(int requestfd) {
	int	bytesRead;
	char	_request[10000] = {0};

	bytesRead = read(requestfd, _request, 8000);
	if (bytesRead < 0)
	{
		close(requestfd);
		throw std::runtime_error("Fail to read client request");
	}
	if (bytesRead == 0) {
		_requestfds.erase(requestfd);
		close(requestfd);
	}
	else {
		std::cout << "Request { " << _request << " }" << std::endl;
		this->_requestfds[requestfd] = _request;
	}
	return (bytesRead);
}

void	Server::respondRequest(int requestfd) {
	std::string	response;

	/* Nesta parte do código, a partir do requestfd, que é uma key value do map
	de response, será chamado uma função de parser para parsear e responder
	o request (que está salvo no map) de acordo com o pedido e as configurações
	registradas do servidor nesta mesma classe.
	Por enquanto, estou apenas devolvendo uma página padrão para saber
	que tudo está funcionando de acordo */
	
	/*Parseamento do request e salva um map comtudo e o body do request*/
	Request _req(_requestfds[requestfd]);
	_req_parsed = _req.getmap();
	_req_body = _req.getbody();
	response += HttpAns::GET_HTML;
	response += _getHtmlIndex();
	write(requestfd, response.c_str(), response.length());
	std::cout << "Response sent\n" << std::endl;
	_requestfds.erase(requestfd);
	close(requestfd);
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}
