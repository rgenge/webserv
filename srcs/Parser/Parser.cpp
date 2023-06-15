#include "Parser.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>

Parser::Parser() {
}

Parser::Parser(std::string configFilePath) : _configFilePath(configFilePath), _status(SERVER) {
}

Parser::~Parser() {
	if (this->_configFileStream.is_open())
		this->_configFileStream.close();
}

std::queue<t_serverConfig>	Parser::parseConfig(void) {
	this->_openConfigFile();
	this->_readConfigFile();
	// this->_tokenizeConfigFile();

	for (t_linesIterator it = _configFileLines.begin(); it != _configFileLines.end(); ++it) {
		if (_isLineInvalid(*it))
			continue ;
		if (_status == SERVER)
			_parseConfig(it);
		else if (_status == CONFIG)
			_parseServerConfig(it, _configFileLines.end());
	}
	return (this->_serverConfigs);
}

void	Parser::_parseConfig(t_linesIterator &it) {
	std::string			serverBuffer;
	std::string			token;
	std::istringstream	lineStream;

	_jumpInvalidLines(it);
	lineStream.str(*it);
	lineStream >> token;
	if (token != "server")
		throw Parser::ParserException("failed to find server configuration");
	++it;
	_jumpInvalidLines(it);
	if (*it != "{")
		throw Parser::ParserException("failed to find server configuration");
	_status = CONFIG;
}

void	Parser::_parseServerConfig(t_linesIterator &it, t_linesIterator end) {
	t_serverConfig		serverConfig;
	std::istringstream	lineStream;
	std::string			token;

	while (*it != "}" && it != end) {
		lineStream.str(*it);
		lineStream >> token;
		if (token[0] == '#')
			;
		else if (token == "port")
			_parsePort(lineStream, serverConfig);
		else if (token == "server_name")
			_parseServerName(lineStream, serverConfig);
		else if (token == "root")
			_parseOneStringParams(lineStream, serverConfig.root);
		else if (token == "index")
			_parseOneStringParams(lineStream, serverConfig.index);
		else if (token == "error_page")
			_parseErrorPages(lineStream, serverConfig);
		else if (token == "body_size_limit")
			_parseLimit(lineStream, serverConfig);
		else if (token == "url")
			_parseUrl(lineStream, serverConfig, it);
		else if ((*it).find_first_not_of("\n ") == std::string::npos)
			;
		else
			throw Parser::ParserException("invalid server config input");
		++it;
		lineStream.clear();
		token.clear();
	}
	if (it == end)
		throw Parser::ParserException("Invalid config file: ");
	_status = SERVER;
	_serverConfigs.push(serverConfig);
}

void	Parser::_parsePort(std::istringstream &lineStream, t_serverConfig &serverConfig) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing port number");
	for (size_t i = 0; i < token.size(); i++) {
		if (std::isdigit(token[i]) == false )
			throw Parser::ParserException("port should be a number");
	}
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("Port accepts only one argument");
	serverConfig.port = std::atoi(token.c_str());
}

void	Parser::_parseOneStringParams(std::istringstream &lineStream, std::string &param) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing argument");
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("invalid argument");
	param = token;
}

void	Parser::_parseServerName(std::istringstream &lineStream, t_serverConfig &serverConfig) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing server_name argument");
	while (lineStream) {
		serverConfig.serverNames.insert(token);
		lineStream >> token;
	}
}

void	Parser::_parseErrorPages(std::istringstream &lineStream, t_serverConfig &serverConfig) {
	std::string	token;
	int			error;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing error_page code");
	for (size_t i = 0; i < token.size(); i++) {
		if (std::isdigit(token[i]) == false )
			throw Parser::ParserException("error code should be a number");
	}
	error = std::atoi(token.c_str());
	lineStream >> token;
	if (!lineStream)
		throw Parser::ParserException("missing error_page file");
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("error_page accepts only 2 arguments");
	serverConfig.errorPages.insert(std::pair<int, std::string>(error, token));
}

void	Parser::_parseLimit(std::istringstream &lineStream, t_serverConfig &serverConfig) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing body_size_limit argument");
	for (size_t i = 0; i < token.size(); i++) {
		if (std::isdigit(token[i]) == false )
			throw Parser::ParserException("body_size_limit should be a number");
	}
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("body_size_limit accepts only one argument");
	serverConfig.bodySizeLimit = std::atoi(token.c_str());
}

void	Parser::_parseUrl(std::istringstream &lineStream, t_serverConfig &serverConfig, t_linesIterator &it) {
	std::string	token;
	t_route		route;
	std::string	routeName;

	route.dirList = false;
	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing url path argument");
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("too many url arguments");
	routeName = token;
	it++;
	_jumpInvalidLines(it);
	if (*it != "{")
		throw Parser::ParserException("missing url");
	it++;
	while (*it != "}") {
		// std::cout << *it << std::endl;
		lineStream.clear();
		lineStream.str(*it);
		lineStream >> token; 
		if (token[0] == '#')
			;
		else if (token == "methods")
			_parseMethods(lineStream, route);
		else if (token == "root")
			_parseOneStringParams(lineStream, route.root);
		else if (token == "index")
			_parseOneStringParams(lineStream, route.index);
		else if (token == "dir_list")
			_parseDirList(lineStream, route);
		else if (token == "cgi")
			_parseOneStringParams(lineStream, route.cgi);
		else if (token == "location")
			_parseOneStringParams(lineStream, route.location);
		else if (token == "upload")
			_parseOneStringParams(lineStream, route.uploadPath);
		++it;
		token.clear();
	}
	serverConfig.routes.insert(std::pair<std::string, t_route>(routeName, route));
}

void	Parser::_parseMethods(std::istringstream &lineStream, t_route &route) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing methods argument");
	while (lineStream) {
		if (token == "GET" || token == "POST" || token == "DELETE")
			route.httpMethods.insert(token);
		else
			throw Parser::ParserException("invalid http method");
		lineStream >> token;
	}
}

void	Parser::_parseDirList(std::istringstream &lineStream, t_route &route) {
	std::string	token;

	lineStream >> token;
	if (token.empty())
		throw Parser::ParserException("missing dir_list argument");
	lineStream >> token;
	if (lineStream)
		throw Parser::ParserException("index accepts only one argument");
	if (token == "on")
		route.dirList = true;
	else if (token == "off")
		route.dirList = false;
}

void	Parser::_readConfigFile(void) {
	std::string	line;

	while (std::getline(_configFileStream, line)) {
		for (size_t i = 0; i < line.size(); i++) {
			if (line[i] == '#') {
				_configFileLines.push_back(line.substr(i));
				line.erase(i);
				break ;
			}
			if (line[i] == '{' || line[i] == '}') {
				if (i != 0)
					_configFileLines.push_back(line.substr(0, i));
				_configFileLines.push_back(line.substr(i, 1));
				line.erase(0, i + 1);
				i = 0;
			}
		}
		if (line.empty() == false)
			_configFileLines.push_back(line);
	}
}

void	Parser::_validateConfigFileBraces(void) {
	int	braces = 0;

	for (t_linesIterator it = _configFileLines.begin(); it != _configFileLines.end(); ++it) {
		if (braces < 0)
			break ;
		else if (*it == "{")
			braces++;
		else if (*it == "}")
			braces--;
	}
	if (braces != 0)
		throw Parser::ParserException("invalid config file");
}

void	Parser::_validateConfigFileName(void) {
	size_t	index;
	
	index = _configFilePath.find_first_of(".conf");
	if (index != (_configFilePath.size() - 5))
		throw Parser::ParserException("webserv only accepts '.conf' file");
}

void	Parser::_openConfigFile(void) {
	this->_validateConfigFileName();
	this->_configFileStream.open(_configFilePath.c_str(), std::ios::in);
	if (this->_configFileStream.fail())
		throw Parser::ParserException("In openning '" + _configFilePath + "'");
}

bool	Parser::_isLineInvalid(std::string const &line) {
	if (line.find_first_not_of("\n ") == std::string::npos)
		return (true);
	if (line.empty())
		return (true);
	if (line[0] == '#')
		return (true);
	return (false);
}

void	Parser::_jumpInvalidLines(t_linesIterator &it) {
	while (_isLineInvalid(*it)) {
		it++;
	}
}

// ParserException

Parser::ParserException::ParserException(std::string const &errorMessage) {
	_errorMessage = new char[errorMessage.size() + 1];
	errorMessage.copy(_errorMessage, errorMessage.size());
	_errorMessage[errorMessage.size()] = 0;
}

Parser::ParserException::~ParserException() throw() {
	delete [] _errorMessage;
}

const char *Parser::ParserException::what() const throw() {
	return (_errorMessage);
}