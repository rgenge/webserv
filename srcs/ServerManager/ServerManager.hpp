#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Server.hpp"
#include "CgiHandler.hpp"
# include <vector>
# include <set>
# include <queue>
# include <poll.h>

class ServerManager {

	public:

		ServerManager();
		~ServerManager();

		void	initialize(void);
		void	createServers(std::queue<t_serverConfig> &serverConfigs);
		Server	&getServer(int index);
		
		static int	active;

	private:

		std::vector<Server>			_servers;
		std::vector<struct pollfd>	_pollFdsMaster;
		std::set<int>				_requestFds;
		std::set<int>				_respondFds;

		Server	&_getServerBySocketFd(int fd);
		Server	&_getServerByRequestFd(int fd);

		void	_addFdToPoll(int fd, short flag);
		void	_initializeServers();
		void	_acceptConnecitons(void);
		void	_getServersRequests(void);
		void	_respondServerRequests(void);
		void	_cleanPollfds(void);

		bool	_isFdNotReadable(struct pollfd pollfd);
		bool	_isFdNotWritable(struct pollfd pollfd);
		bool	_hasNotReceivedConnection(struct pollfd pollfd);

		void	_addServer(Server server);
		void	_shutdownServers(void);
};

#endif /* SERVERMANAGER_HPP */
