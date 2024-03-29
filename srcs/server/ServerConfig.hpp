#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include "webserv.hpp"
# include <set>
# include <map>

class ServerConfig {

	public:

		ServerConfig();
		ServerConfig(t_serverConfig const &serverConfig);
		ServerConfig(t_serverConfig const &serverConfig, t_route const &route);
		ServerConfig(ServerConfig const &rhs);
		~ServerConfig();

		ServerConfig&	operator=(ServerConfig const &rhs);

		int									getPort(void) const;
		int									getBodySizeLimit(void) const;
		const std::string					&getRoot(void) const;
		const std::string					&getIndex(void) const;
		const std::string					&getCgi(std::string const &key) const;
		const std::set<std::string>			&getServerNames(void) const;
		const std::string					&getErrorPage(int errorCode);
		const std::set<std::string>			&getHttpMethods(void) const;
		const std::string					&getUploadPath(void) const;
		const std::string					&getRedirect(void) const;
		bool								getDirList(void) const;

	private:

		int									_port;
		int									_bodySizeLimit;
		std::string							_root;
		std::string							_index;
		std::map<std::string, std::string>	_cgi;
		std::set<std::string>				_serverNames;
		std::map<int, std::string>			_errorPages;
		std::set<std::string>				_httpMethods;
		std::string							_uploadPath;
		std::string							_redirect;
		bool								_dirList;

};

#endif /* SERVERCONFIG_HPP*/
