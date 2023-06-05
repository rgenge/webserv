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

bool	ServerManager::_IsFdNotReadable(struct pollfd pollfd) {
	if ((pollfd.revents & POLLIN)
		&& pollfd.events == (POLLIN | POLLOUT))
		return (false);
	return (true);
}

void	ServerManager::_getServersRequests(void) {
	for (size_t i = 0; i < _numberOfPollFds; i++) {
		if (_IsFdNotReadable(this->_pollfds[i])) {
			continue ;
		}
		if (_getServerByRequestFd(this->_pollfds[i].fd).getRequest(this->_pollfds[i].fd) == 0)
			_pollFdsMaster.erase(_pollFdsMaster.begin() + i);
		this->_pollfds[i].revents = 0;
	}
}

void	ServerManager::_respondServerRequests(void) {
	for (size_t i = 0; i < _numberOfPollFds; i++) {
		if (this->_pollfds[i].revents & POLLOUT){
			_getServerByRequestFd(this->_pollfds[i].fd).respondRequest(this->_pollfds[i].fd);
			_pollFdsMaster.erase(_pollFdsMaster.begin() + i);
			this->_pollfds[i].revents = 0;
		}
	}
}

bool	ServerManager::_acceptConnecitons(void) {
	int		requestfd;
	bool	_hasAcceptedConnections = false;

	requestfd = -1;
	for (size_t i = 0; i < this->_numberOfPollFds; i++) {
		if ((_pollfds[i].revents == POLLIN) && (_pollfds[i].events == POLLIN)) {
			requestfd = _getServerBySocketFd(_pollfds[i].fd).acceptConnection();
			if (requestfd < 0)
				throw std::runtime_error("In accepting connection");
			std::cout << "Request accepted: " << requestfd << "\n" << std::endl;
			_addFdToPoll(requestfd, POLLIN | POLLOUT);
			_pollfds[i].revents = 0;
			_getServerBySocketFd(_pollfds[i].fd).addRequestfd(requestfd, "");
			_hasAcceptedConnections = true;
		}
	}
	return (_hasAcceptedConnections);
}

void	ServerManager::initialize(void) {
	_makeServers();
	for (size_t i = 0; i < this->_servers.size(); i++) {
		_addFdToPoll(_servers[i].getSocketFd(), POLLIN);
	}
	while (1) {
		_createPollFds();
		_numberOfPollFds = this->_pollFdsMaster.size();
		std::cout << "NUMBER OF POLLS: " << _numberOfPollFds << std::endl;
		std::cout << "Waiting for connection" << std::endl;
		if (poll(this->_pollfds, _numberOfPollFds, -1) < 0)
			throw std::runtime_error("Failed to poll");
		if (_acceptConnecitons()) {
			delete [] this->_pollfds;
			continue ;
		}
		_getServersRequests();
		_respondServerRequests();
		delete [] this->_pollfds;
	}
}

void	ServerManager::_addFdToPoll(int fd, short flag) {
	struct pollfd	poll;

	poll.fd = fd;
	poll.events = flag;
	poll.revents = 0;
	this->_pollFdsMaster.push_back(poll);
}

void	ServerManager::_createPollFds(void) {
	this->_pollfds = new struct pollfd[_pollFdsMaster.size()];
	for (size_t i = 0; i < _pollFdsMaster.size(); i++) {
		this->_pollfds[i] = _pollFdsMaster[i];
	}
}

Server&	ServerManager::_getServerBySocketFd(int fd) {
	/* Essa função não é ideal. Apesar de ela nunca falhar, pois sempre vai ser chamada com
	um FD válido e retornar um Server válido. O problema da função é que ela sempre espera
	o retorno de um Server, sendo necessário um retorno fora do loop (que não faz sentido)*/
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).getSocketFd() == fd)
			return (*it);
	}
	std::cout << "ERROR SOCKET FD!" << std::endl;
	return (*_servers.begin()); // esse retorno não faz sentido, é apenas para a flag de -Wextra
}

Server&	ServerManager::_getServerByRequestFd(int fd) {
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).hasRequestFd(fd))
			return (*it);
	}
	std::cout << "ERROR REQUEST FD!" << std::endl;
	return (*_servers.begin()); // esse retorno não faz sentido, é apenas para a flag de -Wextra
}
