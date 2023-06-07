#include "ServerManager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>

ServerManager::ServerManager() {
	
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

bool	ServerManager::_isFdNotReadable(struct pollfd fd) {
	if (fd.fd == -1)
		return (true);
	if (_requestFds.find(fd.fd) == _requestFds.end())
		return (true);
	if ((fd.revents & POLLIN) && fd.events == (POLLIN | POLLOUT))
		return (false);
	return (true);
}

void	ServerManager::_getServersRequests(void) {
	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it < _pollFdsMaster.end(); it++) {
		if (_isFdNotReadable(*it))
			continue ;
		if (_getServerByRequestFd((*it).fd).getRequest((*it).fd) == 0)
			(*it).fd = -1;
		else {
			_requestFds.erase((*it).fd);
			_respondFds.insert((*it).fd);
		}
	}
}

bool	ServerManager::_isFdNotWritable(struct pollfd pollfd) {
	if (pollfd.fd == -1)
		return (true);
	if (!(pollfd.revents & POLLOUT))
		return (true);
	if (_respondFds.find(pollfd.fd) == _respondFds.end())
		return (true);
	return (false);
}

void	ServerManager::_respondServerRequests(void) {
	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it < _pollFdsMaster.end(); it++) {
		if (_isFdNotWritable((*it)))
			continue ;
		_getServerByRequestFd((*it).fd).respondRequest((*it).fd);
		_respondFds.erase((*it).fd);
		(*it).fd = -1;
	}
}

bool	ServerManager::_hasNotReceivedConnection(struct pollfd pollfd) {
	if (pollfd.fd == -1)
		return (true);
	if (pollfd.events != POLLIN)
		return (true);
	if (pollfd.revents != POLLIN)
		return (true);
	return (false);
}

void	ServerManager::_acceptConnecitons(void) {
	int		requestfd;

	requestfd = -1;
	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it < _pollFdsMaster.end(); it++) {
		if (_hasNotReceivedConnection((*it)))
			continue ;
		requestfd = _getServerBySocketFd((*it).fd).acceptConnection();
		if (requestfd < 0)
			throw std::runtime_error("In accepting connection");
		std::cout << "Request accepted!" << std::endl;
		(*it).revents = 0;
		_getServerBySocketFd((*it).fd).addRequestfd(requestfd, "");
		_addFdToPoll(requestfd, POLLIN | POLLOUT);
		_requestFds.insert(requestfd);
		it = _pollFdsMaster.begin();
	}
}

void	ServerManager::initialize(void) {
	_makeServers();
	for (size_t i = 0; i < this->_servers.size(); i++) {
		_addFdToPoll(_servers[i].getSocketFd(), POLLIN);
	}
	std::cout << "Waiting for connection" << std::endl;
	while (1) {
		if (poll(_pollFdsMaster.data(), _pollFdsMaster.size(), -1) < 0)
			throw std::runtime_error("Failed to poll");
		_acceptConnecitons();
		_getServersRequests();
		_respondServerRequests();
		_cleanPollfds();
	}
}

void	ServerManager::_addFdToPoll(int fd, short flag) {
	struct pollfd	poll;

	poll.fd = fd;
	poll.events = flag;
	poll.revents = 0;
	this->_pollFdsMaster.push_back(poll);
}

void	ServerManager::_cleanPollfds(void) {
	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it < _pollFdsMaster.end(); it++) {
		if ((*it).fd == -1) {
			_pollFdsMaster.erase(it);
			it = _pollFdsMaster.begin();
		}
		(*it).revents = 0;
	}
}

Server&	ServerManager::_getServerBySocketFd(int fd) {
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).getSocketFd() == fd)
			return (*it);
	}
	throw std::runtime_error("Couldn't find server");
}

Server&	ServerManager::_getServerByRequestFd(int fd) {
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).hasRequestFd(fd))
			return (*it);
	}
	throw std::runtime_error("Couldn't find server");
}
