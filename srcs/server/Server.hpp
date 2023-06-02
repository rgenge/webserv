#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"
# include <vector>

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

		void	handleRequest(int fd);

	private:

		// Socket		_socket;
		// int			_requestfd;
		// std::vector<int>	_requestfds;
		char				_request[10000];
		std::string			_response;

		void	_getHtmlIndex(void);

};

#endif /* SERVER_HPP */
