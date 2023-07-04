#include "Server.hpp"

Server::Server(t_serverConfig const &config) : Socket(config.port, 10), _serverConfig(config), _flag(0) {
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
		for (int i = 0; i < bytesRead; i++)
		{
			this->_requestData.push_back(static_cast<unsigned char>(_request[i]));
			std::cout << _request[i];
		}
	}

	return (bytesRead);
}

void	Server::respondRequest(int requestfd) {
	std::string	response;
	_flag++;
	/*Parseamento do request e salva um map comtudo e o body do request*/
	Request _req(_requestfds[requestfd]);
	_req_parsed = _req.getMap();
	_req_body = _req.getBody();
	/*Iniciando o server com os dados do path selecionado*/
	if (_req_parsed["Path"] == "/")
	{
		_configs = ServerConfig(_serverConfig);
		_url_path = _req_parsed["Path"];
	}
	else if (_serverConfig.routes.find(_req_parsed["Path"]) == _serverConfig.routes.end())
		std::cerr << "Error 404" << std::endl;
	else
	{
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[_req_parsed["Path"]]);
		_url_path = _req_parsed["Path"];
	}
	/*Iniciando o response*/
	Response res_struct(_res_param, _req_parsed, _serverConfig, _actual_root, _configs, _url_path, this->_requestData);
	res_struct.init(_flag);
	/*response recebe o header e body da resposta e escreve no fd*/
	response = res_struct.getResponse();
	response += res_struct.getBody();
	write(requestfd, response.c_str(), response.length());
	_res_param.clear();
	_requestfds.erase(requestfd);
	close(requestfd);
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}

