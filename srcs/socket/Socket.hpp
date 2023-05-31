#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <sys/socket.h>
# include <netinet/in.h>

class Socket {

	public:

		Socket(int port, int backlog);
		~Socket();

		int		acceptConnection(void);
		void	initialize(void);
	
	private:

		struct sockaddr		*_socketaddr;
		int					_serverfd;
		long				_valread;
		struct sockaddr_in	_address;
		int					_addrlen;
		std::string			_response;
		int					_port;
		int					_backlog;
		socklen_t			*_socketAddrlen;

		int		_listen(int backlog);
		int		_bindSocket(void);
};

#endif /* SOCKET_HPP */
