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

typedef std::vector<std::string>::iterator t_tokensIterator;

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

		void	_openConfigFile(void);
		void	_readConfigFile(void);
		// void	_tokenizeConfigFile(void);
		void	_validateConfigFileName();
		void	_parseConfig(t_tokensIterator &it);
		void	_parseServerConfig(t_tokensIterator &it);
		void	_parsePort(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseServerName(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseRoot(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseIndex(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseErrorPages(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseLimit(std::istringstream &lineStream, t_serverConfig &serverConfig);

		void	_parseUrl(std::istringstream &lineStream, t_serverConfig &serverConfig, t_tokensIterator &it);
		void	_parseMethods(std::istringstream &lineStream, t_route &route);
		void	_parseDirList(std::istringstream &lineStream, t_route &route);
		void	_parseUpload(std::istringstream &lineStream, t_route &route);

		void	_parseOneStringParams(std::istringstream &lineStream, std::string &param);
};

#endif /* PARSER_HPP */
