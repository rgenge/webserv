#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {
}

ServerConfig::ServerConfig(t_serverConfig const &serverConfig) {
	_port = atoi(serverConfig.port.c_str());
	_bodySizeLimit = serverConfig.bodySizeLimit;
	_root = serverConfig.root;
	_index = serverConfig.index;
	_serverNames = serverConfig.serverNames;
	_errorPages = serverConfig.errorPages;
	_httpMethods.insert("GET");
	_dirList = false;
}

ServerConfig::ServerConfig(t_serverConfig const &serverConfig, t_route const &route) {
	_port = atoi(serverConfig.port.c_str());
	_bodySizeLimit = serverConfig.bodySizeLimit;
	_serverNames = serverConfig.serverNames;
	_errorPages = serverConfig.errorPages;
	_dirList = route.dirList;
	_uploadPath = route.uploadPath;
	_cgi = route.cgi;
	_redirect = route.redirect;
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
	_redirect = rhs._redirect;
	return (*this);
}

int	ServerConfig::getPort(void) const {
	return (this->_port);
}

int	ServerConfig::getBodySizeLimit(void) const {
	return (this->_bodySizeLimit);
}

const std::string	&ServerConfig::getRoot(void) const {
	return (this->_root);
}

const std::string	&ServerConfig::getIndex(void) const {
	return (this->_index);
}

const std::vector<std::string>	&ServerConfig::getCgi(void) const {
	return (this->_cgi);
}

const std::set<std::string>	&ServerConfig::getServerNames(void) const {
	return (this->_serverNames);
}

const std::string	&ServerConfig::getErrorPage(int errorCode) {
	return (this->_errorPages[errorCode]);
}

const std::set<std::string>	&ServerConfig::getHttpMethods(void) const {
	return (this->_httpMethods);
}


const std::string	&ServerConfig::getUploadPath(void) const {
	return (this->_uploadPath);
}

bool	ServerConfig::getDirList(void) const {
	return (this->_dirList);
}

const std::string	&ServerConfig::getRedirect(void) const {
	return (this->_redirect);
}
