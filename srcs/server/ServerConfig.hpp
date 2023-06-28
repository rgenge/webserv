#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include "webserv.hpp"
# include <set>
# include <map>

class ServerConfig {

	public:

		ServerConfig(t_serverConfig const &serverConfig);
		ServerConfig(t_serverConfig const &serverConfig, t_route const &route);
		ServerConfig(ServerConfig const &rhs);
		~ServerConfig();

		ServerConfig&	operator=(ServerConfig const &rhs);

		const int							getPort(void) const;
		const int							getBodySizeLimit(void) const;
		const std::string					&getRoot(void) const;
		const std::string					&getIndex(void) const;
		const std::vector<std::string>		&getCgi(void) const;
		const std::set<std::string>			&getServerNames(void) const;
		const std::map<int, std::string>	&getErrorPages(void) const;
		const std::set<std::string>			&getHttpMethods(void) const;
		const std::string					&getUploadPath(void) const;
		const std::string					getRedirect(void) const;
		const bool							getDirList(void) const;

	private:

		int							_port;
		int							_bodySizeLimit;
		std::string					_root;
		std::string					_index;
		std::vector<std::string>	_cgi;
		std::set<std::string>		_serverNames;
		std::map<int, std::string>	_errorPages;
		std::set<std::string>		_httpMethods;
		std::string					_uploadPath;
		std::string					_redirect;
		bool						_dirList;

};

#endif /* SERVERCONFIG_HPP*/
