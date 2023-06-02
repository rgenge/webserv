#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <sys/socket.h>
# include <netinet/in.h>

class Socket {

	public:

		Socket();
		Socket(int port, int backlog);
		Socket(Socket const &rhs);
		~Socket();

		Socket&	operator=(Socket const &rhs);

		int		acceptConnection(void);
		void	initialize(void);
		void	closeSocket(void) const;
		int		getSocketFd(void) const;
	
	protected:

		int					_socketfd;
		int					_addrlen;
		int					_port;
		int					_backlog;
		socklen_t			*_socketAddrlen;
		struct sockaddr		*_socketaddr;
		struct sockaddr_in	_address;

		int		_listen(int backlog);
		int		_bindSocket(void);
};

#endif /* SOCKET_HPP */
