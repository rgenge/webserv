#include "Server.hpp"

Server::Server(t_serverConfig const &config) : Socket(10, config.port), _serverConfig(config) {
}

Server::~Server() {
}

Server::Server(Server const &rhs) : Socket(10, rhs._port) {
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

requestStatus	Server::_checkRequestStatus(std::string const &_request) {
	size_t		contentLengthIndex;
	size_t		contentLength;
	size_t		bodyPosition;
	std::string	body;

	contentLengthIndex = _request.find("Content-Length: ");
	if (contentLengthIndex == std::string::npos)
		return (DONE);
	contentLengthIndex += 15;
	contentLength = std::atoi(&_request.c_str()[contentLengthIndex]);
	bodyPosition = _request.find(DCRLF) + 4;
	body = _request.substr(bodyPosition);
	if (body.size() != contentLength)
		return (PROCESSING);
	return (DONE);
}

requestStatus	Server::getRequest(int requestfd) {
	int	bytesRead;
	char	_request[10000] = {0};

	bytesRead = read(requestfd, _request, 8000);
	if (bytesRead < 0) {
		close(requestfd);
		throw std::runtime_error("Fail to read client request");
	}
	if (bytesRead == 0) {
		_requestfds.erase(requestfd);
		close(requestfd);
		return (ERROR);
	}
	else 
		this->_requestfds[requestfd] += _request;
	return (_checkRequestStatus(this->_requestfds[requestfd]));
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
	_res_param.clear();
	_req_parsed.clear();
	_requestfds.erase(requestfd);
	close(requestfd);
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}

