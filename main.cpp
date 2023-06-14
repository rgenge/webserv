#include <iostream>
#include "ServerManager.hpp"

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	ServerManager	serverManager;

	serverManager.addServer(Server(8080));
	serverManager.addServer(Server(8081));
	serverManager.addServer(Server(9000));
	try {
		serverManager.initialize();
	}	catch (std::exception &e) {
		std::cout << "Error to initialize webserv: " << e.what() << std::endl;
	}
	return (0);
}
