#include "ServerManager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <cerrno>

int ServerManager::active = 1;
void	signalHandler(int sig);

ServerManager::ServerManager() {
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
}

ServerManager::~ServerManager() {
	this->_shutdownServers();
}

void	signalHandler(int sig) {
	if (sig == SIGINT)
		ServerManager::active = 0;
}

void	ServerManager::_shutdownServers(void) {
	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it != _pollFdsMaster.end(); it++)
		close((*it).fd);
}

void	ServerManager::_addServer(Server server) {
	this->_servers.push_back(server);
}

Server&	ServerManager::getServer(int index) {
	return (_servers[index]);
}

void	ServerManager::_initializeServers(void) {
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
	requestStatus	status;

	for (std::vector<struct pollfd>::iterator it = _pollFdsMaster.begin(); it < _pollFdsMaster.end(); it++) {
		if (_isFdNotReadable(*it))
			continue ;
		status = _getServerByRequestFd((*it).fd).getRequest((*it).fd);
		if (status == ERROR)
			(*it).fd = -1;
		else if (status == DONE) {
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

void	ServerManager::_respondServerRequests() {
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
		std::cout << "Connection accepted!" << std::endl;
		(*it).revents = 0;
		_getServerBySocketFd((*it).fd).addRequestfd(requestfd, std::vector<unsigned char>());
		_addFdToPoll(requestfd, POLLIN | POLLOUT);
		_requestFds.insert(requestfd);
		it = _pollFdsMaster.begin();
	}
}

void	ServerManager::initialize(void) {
	_initializeServers();
	for (size_t i = 0; i < this->_servers.size(); i++)
		_addFdToPoll(_servers[i].getSocketFd(), POLLIN);
	std::cout << "Waiting for connection" << std::endl;
	while (1) {
		if (active == 0)
			break ;
		if (poll(_pollFdsMaster.data(), _pollFdsMaster.size(), -1) < 0)
		{
			if (errno == EINTR)
				break ;
			throw ServerManager::ServerManagerException("Failed to poll");
		}
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
	throw ServerManager::ServerManagerException("Couldn't find server");
}

Server&	ServerManager::_getServerByRequestFd(int fd) {
	for (std::vector<Server>::iterator it = _servers.begin(); it < _servers.end(); it++) {
		if ((*it).hasRequestFd(fd))
			return (*it);
	}
	throw ServerManager::ServerManagerException("Couldn't find server");
}

void	ServerManager::createServers(std::queue<t_serverConfig> &serverConfigs) {
	t_serverConfig	config;

	while (serverConfigs.size() != 0) {
		_addServer(Server(serverConfigs.front()));
		serverConfigs.pop();
	}
}

// ServerManagerException

ServerManager::ServerManagerException::ServerManagerException(std::string const &errorMessage) {
	std::string	_ServerManagerError("ServerManager: ");
	_ServerManagerError += errorMessage;
	_errorMessage = new char[_ServerManagerError.size() + 1];
	_ServerManagerError.copy(_errorMessage, _ServerManagerError.size());
	_errorMessage[_ServerManagerError.size()] = 0;
}

ServerManager::ServerManagerException::~ServerManagerException() throw() {
	delete [] _errorMessage;
}

const char	*ServerManager::ServerManagerException::what() const throw() {
	return (this->_errorMessage);
}
