#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <iostream>
# include <map>
# include <set>

typedef struct s_route
{
	std::set<std::string>	httpMethods;
	std::string				index;
	std::string				root;
	bool					dirList;
	std::string				cgi;
	std::string				uploadPath;
}			t_route;

typedef struct s_serverConfig
{
	int									port;
	std::string							root;
	std::string							index;
	std::set<std::string>				serverNames;
	std::map<int, std::string>			errorPages;
	int									bodySizeLimit;
	std::map<std::string, t_route>		routes;
}			t_serverConfig;

#endif /* WEBSERV_HPP*/
