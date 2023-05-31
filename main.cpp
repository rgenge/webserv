#include "Socket.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	Socket		socket(8080, 10);
	int			requestfd;
	char		request[10000] = {0};
	std::string	response;

	(void)argc;
	(void)argv;
	response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 1024\nCache-Control: no-cache\nLast-Modified: Wed, 29 May 2023 12:00:00 GMT\n\n Hello world!";
	socket.initialize();
	while (1) {
		std::cout << "Waiting for connection" << std::endl;
		try {
			requestfd = socket.acceptConnection();

		} catch (std::exception &e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
		read(requestfd, request, 10000);
		std::cout << request << std::endl;
		write(requestfd, response.c_str(), response.length());
		std::cout << "Response sent" << std::endl;
		close(requestfd);
	}
	return (0);
}