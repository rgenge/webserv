#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfig.hpp"
# include "webserv.hpp"
# include <map>
# include <sys/stat.h>
# include <sys/types.h>
# include <iostream>
# include <unistd.h>
# include <fstream>
# include <sstream>
# include <string.h>

class Server : public Socket {

	public:

		Server(t_serverConfig const &config);
		Server(Server const &rhs);
		~Server();

		Server&	operator=(Server const &rhs);

		int		getRequest(int fd);
		void	respondRequest(int fd);
		bool	hasRequestFd(int i);
		void	addRequestfd(int requestfd, std::string requestMessage);
		void	locationCheck();
	private:

		Server();
		std::map <std::string, std::string>	_res_param;
		std::map<int, std::string>			_requestfds;
		std::map <std::string, std::string>	_req_parsed;
		std::string							_req_body;
		t_serverConfig						_serverConfig;
		int									_flag;
		std::string							_actual_root;
		ServerConfig						_configs;
		std::string							_url_path;

};

#endif /* SERVER_HPP */
