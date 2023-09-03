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
		void				addRequestfd(int requestfd, std::vector<unsigned char> requestMessage);

		static std::map<int, std::vector <unsigned char> >	requestComplete;
		static bool											endChunk;
		static bool											isChunk;
		static bool											firstChunk;
	private:

		Server();
		std::map<int, std::vector <unsigned char> >	_requestfds;
		t_serverConfig								_serverConfig;
		std::string									_url_path;
		std::string									_actual_root;

		requestStatus	_checkRequestStatus(std::vector<unsigned char> const &_request);
		requestStatus	_parseChunk(int requestfd);
		size_t			_findSequenceVector(std::vector<unsigned char> const &vector, std::string const sequence);
		void			_respondInternalServerError(int requestfd);

};

#endif /* SERVER_HPP */
