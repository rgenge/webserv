#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <string>
# include <iostream>
# include <map>
# include <set>
# include <vector>
# include<cstdlib>

typedef struct s_route
{
	std::set<std::string>					httpMethods;
	std::string								index;
	std::string								root;
	std::map<std::string, std::string>		cgi;
	std::string								uploadPath;
	std::string								redirect;
	bool									dirList;
}			t_route;

typedef struct s_serverConfig
{
	std::string								port;
	std::string								root;
	std::string								index;
	std::set<std::string>					serverNames;
	std::map<int, std::string>				errorPages;
	std::map<std::string, std::string>		cgi;
	int										bodySizeLimit;
	std::map<std::string, t_route>			routes;
}			t_serverConfig;

#endif /* WEBSERV_HPP*/
