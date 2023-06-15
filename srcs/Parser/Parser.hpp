#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>
# include <queue>
# include <fstream>
# include "webserv.hpp"

enum e_parserStatus {
	SERVER = 1,
	CONFIG,
	URL,
};

typedef std::vector<std::string>::iterator t_linesIterator;

class Parser {

	public:

		Parser(std::string configFilePath);
		~Parser();

		std::queue<t_serverConfig>	parseConfig(void);

	private:

		Parser();

		std::string					_configFilePath;
		std::queue<t_serverConfig>	_serverConfigs;
		std::ifstream				_configFileStream;
		std::vector<std::string>	_configFileLines;
		std::vector<std::string>	_configTokens;
		int							_status;

		// file handle functions 
		void	_validateConfigFileName();
		void	_openConfigFile(void);
		void	_readConfigFile(void);
		// void	_tokenizeConfigFile(void);
		
		// Parse server configuration functions
		void	_parseConfig(t_linesIterator &it);
		void	_parseServerConfig(t_linesIterator &it);
		void	_parsePort(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseServerName(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseErrorPages(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseLimit(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseOneStringParams(std::istringstream &lineStream, std::string &param);

		// Parse server URL configuration functins
		void	_parseUrl(std::istringstream &lineStream, t_serverConfig &serverConfig, t_linesIterator &it);
		void	_parseMethods(std::istringstream &lineStream, t_route &route);
		void	_parseDirList(std::istringstream &lineStream, t_route &route);

		// Parse util functions

		// Verify if a given line is a comment or empty (contains only spaces)
		bool	_isLineInvalid(std::string const &line);
		void	_jumpInvalidLines(t_linesIterator &it);
};

#endif /* PARSER_HPP */
