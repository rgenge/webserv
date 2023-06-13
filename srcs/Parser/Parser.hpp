#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>
# include <stack>
# include <fstream>
# include "webserv.hpp"

class Parser {

	public:

		Parser(std::string configFilePath);
		~Parser();

		std::stack<t_serverConfig>	parseConfig(void);

	private:

		Parser();

		std::string					_configFilePath;
		std::stack<t_serverConfig>	_serverConfigs;
		std::ifstream				_configFileStream;
		std::vector<std::string>	_configFileLines;
		std::vector<std::string>	_configTokens;

		void	_openConfigFile(void);
		void	_readConfigFile(void);
		void	_tokenizeConfigFile(void);
		void	_validateConfigFileName();


};

#endif /* PARSER_HPP */
