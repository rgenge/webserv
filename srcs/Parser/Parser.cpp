#include "Parser.hpp"
#include <iostream>
#include <cstring>
#include <sstream>

Parser::Parser() {
}

Parser::Parser(std::string configFilePath) : _configFilePath(configFilePath) {
}

Parser::~Parser() {
	if (this->_configFileStream.is_open())
		this->_configFileStream.close();
}

std::stack<t_serverConfig>	Parser::parseConfig(void) {
	this->_openConfigFile();
	this->_readConfigFile();
	this->_tokenizeConfigFile();

	for (std::vector<std::string>::iterator it = _configTokens.begin(); it != _configTokens.end(); ++it) {
		std::cout << *it << std::endl;
	}
	return (this->_serverConfigs);
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

void	Parser::_tokenizeConfigFile(void) {
	std::string			token;
	std::istringstream	lineStream;

	for (std::vector<std::string>::iterator it = _configFileLines.begin(); it != _configFileLines.end(); ++it) {
		lineStream.clear();
		lineStream.str(*it);
		lineStream >> token;
		while (lineStream) {
			if (token[0] == '#')
				break ;
			_configTokens.push_back(token);
			lineStream >> token;
		}
	}
}

void	Parser::_validateConfigFileName(void) {
	size_t	index;
	
	index = _configFilePath.find_first_of(".conf");
	if (index != (_configFilePath.size() - 5))
		throw std::runtime_error("webserv only accepts '.conf' file");
}

void	Parser::_openConfigFile(void) {
	this->_validateConfigFileName();
	this->_configFileStream.open(_configFilePath.c_str(), std::ios::in);
	if (this->_configFileStream.fail())
		throw std::runtime_error("In openning '" + _configFilePath + "'");
}
