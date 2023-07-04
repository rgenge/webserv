#include <iostream>
#include "ServerManager.hpp"
#include "Parser.hpp"
#include <csignal>
#include <signal.h>

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	ServerManager	serverManager;
	Parser			parserConfig;
	std::queue<t_serverConfig>	serverConfigs;
	try {
		serverConfigs = parserConfig.parseConfig(argv[1]);
		serverManager.createServers(serverConfigs);
		serverManager.initialize();
	}	catch (std::exception &e) {
		std::cout << "Error to initialize webserv: " << e.what() << std::endl;
	}
	return (0);
}
