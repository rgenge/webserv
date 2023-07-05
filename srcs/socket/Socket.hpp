#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>

class Socket {

	public:

		class SocketException : public std::exception {
			
			public:
				SocketException(std::string const &errorMessage);
				virtual ~SocketException() throw();

				virtual const char *what() const throw();
			
			private:
				
				char	*_errorMessage;
		};

		Socket(int backlog, std::string const &port);
		Socket(Socket const &rhs);
		~Socket();

		Socket&	operator=(Socket const &rhs);

		int		acceptConnection(void);
		void	initialize(void);
		void	closeSocket(void) const;
		int		getSocketFd(void) const;
	
	protected:

		int						_socketfd;
		int						_optval;
		int						_backlog;
		socklen_t				_addrSize;
		std::string				_port;
		struct addrinfo			_hints;
		struct addrinfo			*_serverInfo;
		struct sockaddr_storage	_clientAddr;

		int		_listen(int backlog);
		int		_bindSocket(void);
};

#endif /* SOCKET_HPP */
