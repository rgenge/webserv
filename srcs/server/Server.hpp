#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"

class Server {
	/* A classe server tbm pode se chamar Webserv, acredito que ela vai ser
	o principal objeto do projeto, criando o socket, fazendo o parser, etc.*/
	public:

		Server();
		Server(int port); 
		/* a ideia é que o construtor do server receba futuramente
			apenas o caminho do arquivo de configuração, realize o parser
			e então sete as configurações do servidor em outras structs
			ou classes privadas */
		~Server();

		void	initialize();

	private:

		Socket		*_socket;
		int			_requestfd;
		char		_request[10000];
		std::string	_response;

		void	_getHtmlIndex(void);

};

#endif /* SERVER_HPP */
