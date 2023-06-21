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

void	Server::locationCheck()
{
	std::map<std::string, t_route>::iterator itr;
	for(itr=_serverConfig.routes.begin();itr!=_serverConfig.routes.end();itr++)
	{
//		std::cout << "root1" << _serverConfig.root << std::endl;
//		std::cout << "root2" << itr->second.root << std::endl;
//		std::cout << "root3" << itr->first << std::endl;
//		std::cout <<" root4 "<< _req_parsed["Path"];
		_server_conf["AutoIndex"] = "";
		_server_conf["AutoIndex" ] ="on";
		if ("/" == _req_parsed["Path"])
		{
			_serverConfig.root = "/index";
		}

		if (itr->first == _req_parsed["Path"])
		{
			_serverConfig.root  = itr->second.root;
			if (itr->second.dirList)
				_server_conf["AutoIndex" ] ="on";
			else
				_server_conf["AutoIndex" ] ="off";
		}
		if (_serverConfig.root != _req_parsed["Path"] && itr->second.root != _req_parsed["Path"] && itr->first == _req_parsed["Path"])
		{
//			std::cout <<" path z antes: "<< _req_parsed["Path"];
			_req_parsed["Path"] ="";
//			std::cout <<" path z: "<< _req_parsed["Path"] <<std::endl;
			break;
		}
		struct stat buf;
		std::string dir = ("." + _serverConfig.root + _req_parsed["Path"]);
		lstat(dir.c_str(), &buf);
		std::cout << "dir : " << dir << std::endl;
		FILE *check_fp = fopen(dir.c_str(), "rb");
		if (!check_fp)
			std::cout << "Error 404" << std::endl;
		else
		{
			fclose(check_fp);
			if (S_ISDIR(buf.st_mode) && dir != "./index/")
			{
				_server_conf["AutoIndex" ] ="on";
				std::cout << "diretório \n";
			}
			else
				_server_conf["AutoIndex" ] ="off";
		}
		if (itr->first == _req_parsed["Path"])
			break;
	}
}
void	Server::respondRequest(int requestfd) {
	std::string	response;

	/*Parseamento do request e salva um map comtudo e o body do request*/
	Request _req(_requestfds[requestfd]);
	_req_parsed = _req.getmap();
	_req_body = _req.getbody();
	Response res_struct;
	//Inserindo dados do server manualmente pra teste
	_server_conf.insert(std::pair<std::string,std::string>("AllowedMethod","GET") );
	_server_conf.insert(std::pair<std::string,std::string>("Index",_serverConfig.index) );
	_server_conf.insert(std::pair<std::string,std::string>("AutoIndex","") );
	/*Iniciando o response*/
	locationCheck();
	_server_conf.insert(std::pair<std::string,std::string>("Root",("." + _serverConfig.root)));
	res_struct.init(_req_parsed, _server_conf);
	/*response recebe o header da resposta*/
	response = res_struct.getResponse();
	/*response recebe o body da resposta*/
	response += res_struct.get_body();
	/*Imprimi o site na tela*/
//	std::cout << response ;
	write(requestfd, response.c_str(), response.length());
	std::cout << "Response sent\n" << std::endl;
	_server_conf.clear();
//	_req_parsed.clear();
	_requestfds.erase(requestfd);
	close(requestfd);
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}
