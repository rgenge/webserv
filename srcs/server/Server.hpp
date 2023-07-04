#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"
# include "webserv.hpp"
# include <map>
# define DCRLF "\r\n\r\n"

enum requestStatus {
	ERROR = 1,
	PROCESSING,
	DONE,
};

class Server : public Socket {

	public:

		Server(t_serverConfig const &config);
		/* a ideia é que o construtor do server receba futuramente
			apenas uma struct (ou uma classe), após o parser das informações
			do arquivo de configuração */
		Server(Server const &rhs);
		~Server();

		Server&	operator=(Server const &rhs);

		requestStatus		getRequest(int fd);
		void				respondRequest(int fd);
		bool				hasRequestFd(int i);
		void				addRequestfd(int requestfd, std::string requestMessage);

	private:

		Server();
		std::map <std::string, std::string>	_server_conf;
		std::map<int, std::string>			_requestfds;
		std::map <std::string, std::string>	_req_parsed;
		std::string							_req_body;
		t_serverConfig						_serverConfig;

		requestStatus _checkRequestStatus(std::string const &_request);

};

#endif /* SERVER_HPP */
