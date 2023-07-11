#include <iostream>
#include "ServerManager.hpp"
#include "Parser.hpp"
#include <csignal>
#include <signal.h>

int main(int argc, char *argv[]) {
	ServerManager	serverManager;
	Parser			parserConfig;
	std::queue<t_serverConfig>	serverConfigs;
	
	if (argc != 2)
	{
		std::cerr << "Error to initialize webserv: invalid number of arguments"<< std::endl;
		return (EXIT_FAILURE);
	}
	try {
		serverConfigs = parserConfig.parseConfig(argv[1]);
		serverManager.createServers(serverConfigs);
		serverManager.initialize();
	}	catch (std::exception &e) {
		std::cout << "Error to initialize webserv\n" << e.what() << std::endl;
	}
	return (0);
}
