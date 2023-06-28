#include "ServerConfig.hpp"

ServerConfig::ServerConfig(t_serverConfig const &serverConfig) {
	_port = serverConfig.port;
	_bodySizeLimit = serverConfig.bodySizeLimit;
	_root = serverConfig.root;
	_index = serverConfig.index;
	_serverNames = serverConfig.serverNames;
	_errorPages = serverConfig.errorPages;
	_httpMethods.insert("GET");
	_dirList = false;
}

ServerConfig::ServerConfig(t_serverConfig const &serverConfig, t_route const &route) {
	_port = serverConfig.port;
	_bodySizeLimit = serverConfig.bodySizeLimit;
	_serverNames = serverConfig.serverNames;
	_errorPages = serverConfig.errorPages;
	_dirList = route.dirList;
	_uploadPath = route.uploadPath;
	_cgi = route.cgi;
	if (route.httpMethods.size() > 0)
		_httpMethods = route.httpMethods;
	else
		_httpMethods.insert("GET");
	if (route.index.empty() == false)
		_index = route.index;
	else
		_index = serverConfig.index;
	if (route.root.empty() == false)
		_root = route.root;
	else
		_root = serverConfig.root;
}

ServerConfig::~ServerConfig() {
}

ServerConfig::ServerConfig(ServerConfig const &rhs) {
	*this = rhs;
}

ServerConfig&	ServerConfig::operator=(ServerConfig const &rhs) {
	if (this == &rhs)
		return (*this);
	_port = rhs._port;
	_bodySizeLimit = rhs._bodySizeLimit;
	_serverNames = rhs._serverNames;
	_errorPages = rhs._errorPages;
	_dirList = rhs._dirList;
	_uploadPath = rhs._uploadPath;
	_httpMethods = rhs._httpMethods;
	_index = rhs._index;
	_root = rhs._root;
	_cgi = rhs._cgi;
	return (*this);
}

const int	ServerConfig::getPort(void) const {
	return (this->_port);
}

const int	ServerConfig::getBodySizeLimit(void) const {
	return (this->_bodySizeLimit);
}

const std::string	&ServerConfig::getRoot(void) const {
	return (this->_root);
}

const std::string	&ServerConfig::getIndex(void) const {
	return (this->_index);
}

const std::string	&ServerConfig::getCgi(void) const {
	return (this->_cgi);
}

const std::set<std::string>	&ServerConfig::getServerNames(void) const {
	return (this->_serverNames);
}

const std::map<int, std::string>	&ServerConfig::getErrorPages(void) const {
	return (this->_errorPages);
}

const std::set<std::string>	&ServerConfig::getHttpMethods(void) const {
	return (this->_httpMethods);
}


const std::string	&ServerConfig::getUploadPath(void) const {
	return (this->_uploadPath);
}

const bool	ServerConfig::getDirList(void) const {
	return (this->_dirList);
}