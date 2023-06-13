#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"
# include <map>

class Server : public Socket {

	public:

		Server();
		Server(int port);
		/* a ideia é que o construtor do server receba futuramente
			apenas uma struct (ou uma classe), após o parser das informações
			do arquivo de configuração */
		Server(Server const &rhs);
		~Server();

		Server&	operator=(Server const &rhs);

		int		getRequest(int fd);
		void	respondRequest(int fd);
		bool	hasRequestFd(int i);
		void	addRequestfd(int requestfd, std::string requestMessage);

	private:
		std::map <std::string, std::string>	_server_conf;
		std::map<int, std::string>	_requestfds;
		std::string	_getHtmlIndex(void);
		std::map <std::string, std::string>	_req_parsed;
		std::string	_req_body;
};

#endif /* SERVER_HPP */
