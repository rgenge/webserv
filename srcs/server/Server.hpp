#ifndef SERVER_HPP
# define SERVER_HPP

# include "Socket.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfig.hpp"
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
		Server(Server const &rhs);
		~Server();

		Server&	operator=(Server const &rhs);

		requestStatus		getRequest(int fd);
		void				respondRequest(int fd);
		bool				hasRequestFd(int i);
		void				addRequestfd(int requestfd, std::string requestMessage);

	private:

		Server();
		std::map<int, std::string>			_requestfds;
		std::map <std::string, std::string>	_req_parsed;
		std::string							_req_body;
		t_serverConfig						_serverConfig;
		std::string							_url_path;
		std::string							_actual_root;
		std::vector<unsigned char>			_requestData;
		requestStatus	_checkRequestStatus(std::string const &_request);
		void			_respondInternalServerError(int requestfd);

};

#endif /* SERVER_HPP */
