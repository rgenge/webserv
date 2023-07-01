#include "Server.hpp"

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
		this->_requestfds[requestfd] = _request;
	}
	return (bytesRead);
}

void	Server::respondRequest(int requestfd) {
	std::string	response;
	/*Parseamento do request e salva um map comtudo e o body do request*/
	Request _req(_requestfds[requestfd]);
	_req_parsed = _req.getMap();
	_req_body = _req.getBody();
	/*Iniciando o response*/
	Response res_struct(_res_param, _req_parsed, _serverConfig, _configs,
		_url_path);
	res_struct.init();
	/*response recebe o header e body da resposta e escreve no fd*/
	response = res_struct.getResponse();
	response += res_struct.getBody();
	write(requestfd, response.c_str(), response.length());
//	_res_param.clear();
	_requestfds.erase(requestfd);
	close(requestfd);
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}

