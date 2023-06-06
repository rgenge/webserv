#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include "Server.hpp"
#include <vector>
#include <poll.h>

class ServerManager {

	public:

		ServerManager();
		~ServerManager();

		void	initialize(void);
		void	addServer(Server server);
		Server	&getServer(int index);
		
	private:

		std::vector<Server>			_servers;
		std::vector<struct pollfd>	_pollFdsMaster;
		// std::vector<int>			_requestfds;

		void	_addFdToPoll(int fd, short flag);
		// void	_removeFdFromPoll(struct pollfd	pollfd);
		void	_makeServers();
		Server	&_getServerBySocketFd(int fd);
		Server	&_getServerByRequestFd(int fd);
		bool	_acceptConnecitons(void);
		void	_getServersRequests(void);
		bool	_IsFdNotReadable(struct pollfd pollfd);
		void	_respondServerRequests(void);
};

#endif /* SERVERMANAGER_HPP */
