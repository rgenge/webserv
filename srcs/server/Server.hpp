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

		// Socket		_socket;
		std::map<int, std::string>	_requestfds;
		// char						_request[10000];
		// std::string					_response;
		// int					_requestfd;

		std::string	_getHtmlIndex(void);
};

#endif /* SERVER_HPP */
