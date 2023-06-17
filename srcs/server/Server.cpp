#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "Request.hpp"
#include "Response.hpp"
#include <string.h>

Server::Server(t_serverConfig const &config) : Socket(config.port, 10), _serverConfig(config) {
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
	this->_requestfds = rhs._requestfds;
	this->_req_parsed = rhs._req_parsed;
	this->_req_body = rhs._req_body;
	this->_serverConfig = rhs._serverConfig;
	return (*this);
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
//		std::cout << "Request { " << _request << " }" << std::endl;
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
	Response res_struct;
	//Inserindo dados do server manualmente pra teste
	_server_conf.insert(std::pair<std::string,std::string>("Root","./index"));
	_server_conf.insert(std::pair<std::string,std::string>("AllowedMethod","GET") );
	_server_conf.insert(std::pair<std::string,std::string>("Index","index.html") );
	_server_conf.insert(std::pair<std::string,std::string>("AutoIndex","on") );
	/*Iniciando o response*/
	res_struct.init(_req_parsed, _server_conf);
	/*response recebe o header da resposta*/
	response = res_struct.getResponse();
	/*response recebe o body da resposta*/
	response += res_struct.get_body();
	/*Imprimi o site na tela*/
	std::cout << response ;
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
