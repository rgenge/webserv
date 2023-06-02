#include "ServerManager.hpp"
#include <iostream>
#include <cstring>

ServerManager::ServerManager() : _pollfds(NULL) {
	
}

ServerManager::~ServerManager() {
}

void	ServerManager::addServer(Server server) {
	this->_servers.push_back(server);
}

Server&	ServerManager::getServer(int index) {
	return (_servers[index]);
}

void	ServerManager::_makeServers(void) {
	for (size_t i = 0; i < this->_servers.size(); i++) {
		this->_servers[i].initialize();
	}
}

void	ServerManager::initialize(void) {
	int		requestfd;
	Server	requestingServer;
	_makeServers();
	for (size_t i = 0; i < this->_servers.size(); i++) {
		_addFdToPoll(_servers[i].getSocketFd());
	}
	_createPollFds();
	while (1) {
		std::cout << "Waiting for connection" << std::endl;
		if (poll(this->_pollfds, _pollFdsMaster.size(), 10000) < 0)
			throw std::runtime_error("Failed to poll");
		for (size_t i = 0; i < _pollFdsMaster.size(); i++) {
			if (_pollfds[i].revents == POLLIN) {
				requestingServer = _getServerByFd(_pollfds[i].fd);
				requestfd = requestingServer.acceptConnection();
				requestingServer.handleRequest(requestfd);
				_pollfds[i].revents = 0;
			}
		}
	}
	delete [] _pollfds;
}

void	ServerManager::_addFdToPoll(int fd) {
	struct pollfd	poll;

	poll.fd = fd;
	poll.events = POLLIN;
	this->_pollFdsMaster.push_back(poll);
}

void	ServerManager::_createPollFds(void) {
	this->_pollfds = new struct pollfd[_pollFdsMaster.size()];
	for (size_t i = 0; i < _pollFdsMaster.size(); i++) {
		this->_pollfds[i] = _pollFdsMaster[i];
	}
}

Server&	ServerManager::_getServerByFd(int fd) {
	/* Essa função não é ideal. Apesar de ela nunca falhar, pois sempre vai ser chamada com
	um FD válido e retornar um Server válido. O problema da função é que ela sempre espera
	o retorno de um Server, sendo necessário um retorno fora do loop (que não faz sentido)*/
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).getSocketFd() == fd)
			return (*it);
	}
	return (*_servers.begin()); // esse retorno não faz sentido, é apenas para a flag de -Wextra
}
