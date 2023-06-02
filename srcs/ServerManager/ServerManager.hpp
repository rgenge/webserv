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
		std::vector<int>			_requestfds;
		struct pollfd				*_pollfds;

		void	_addFdToPoll(int fd);
		void	_createPollFds(void);
		// void	_removeFdFromPoll(struct pollfd	pollfd);
		void	_makeServers();
		Server	&_getServerByFd(int fd);

};

#endif /* SERVERMANAGER_HPP */
