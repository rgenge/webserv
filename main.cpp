#include "Server.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	Server	webserv(8080);

	try {
		webserv.initialize();
	}	catch (std::exception &e) {
		std::cout << "Error to initialize webserv: " << e.what() << std::endl;
	}
	return (0);
}
