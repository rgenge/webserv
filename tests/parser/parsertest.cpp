#include "Parser.hpp"
#include <iostream>

template <typename T>
void	printContainers(T containers) {
	for (typename T::iterator it = containers.begin(); it != containers.end(); ++it) {
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
}

void	printErrorPages(std::map<int, std::string> errorPages) {
	for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
		std::cout << it->first << "\t" << it->second << std::endl;
	}
}

void	printRoutes(t_serverConfig serverConfig) {
	t_route	route;
	for (std::map<std::string, t_route>::iterator it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it) {
		std::cout << "url: " << it->first << std::endl;
		route = it->second;
		std::cout << "\thttpMethods: ";
		printContainers< std::set<std::string> >(route.httpMethods);
		std::cout << "\tlocation: " << route.location << std::endl;
		std::cout << "\tindex: " << route.index << std::endl;
		std::cout << "\troot: " << route.root << std::endl;
		std::cout << "\tdirList: " << route.dirList << std::endl;
		std::cout << "\tcgi: " << route.cgi << std::endl;
		std::cout << "\tuploadPath: " << route.uploadPath << std::endl;
	}
}

void	showServerConfigs(std::queue<t_serverConfig> serverConfigs) {
	t_serverConfig	config;
	while (serverConfigs.size() > 0) {
		config = serverConfigs.front();
		serverConfigs.pop();
		std::cout << "Port: " << config.port << std::endl;
		std::cout << "root: " << config.root << std::endl;
		std::cout << "index: " << config.index << std::endl;
		std::cout << "serverNames: ";
		printContainers< std::set<std::string> >(config.serverNames);
		std::cout << "ErrorPages: ";
		printErrorPages(config.errorPages);
		std::cout << "BodySizeLimit: " << config.bodySizeLimit << std::endl;
		printRoutes(config);
	}
}

int main(int argc, char *argv[]) {
	Parser						parser(argv[1]);
	std::queue<t_serverConfig>	serverConfigs;
	(void)argc;

	try {
		serverConfigs = parser.parseConfig();
		showServerConfigs(serverConfigs);
	} catch(std::exception &e) {
		std::cerr << "test error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
