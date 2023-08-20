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

		class ParserException : public std::exception {

			public:
				
				ParserException(std::string const &errorMessage);
				virtual ~ParserException() throw();
				virtual const char *what() const throw();

			private:

				char *_errorMessage;
		};
		Parser();
		~Parser();

		std::queue<t_serverConfig>	parseConfig(std::string const &configFilePath);

	private:

		std::queue<t_serverConfig>	_serverConfigs;
		std::ifstream				_configFileStream;
		std::vector<std::string>	_configFileLines;
		std::vector<std::string>	_configTokens;
		int							_status;

		// === file handle functions ===

		void	_validateConfigFileName(std::string const &configFilePath);
		void	_openConfigFile(std::string const &configFilePath);
		void	_readConfigFile(void);
		void	_validateConfigFileBraces(void);
		// void	_tokenizeConfigFile(void);
		
		// === Parse server configuration functions ===

		void	_parseConfig(t_linesIterator &it);
		void	_parseServerConfig(t_linesIterator &it);
		void	_parsePort(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseServerName(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseErrorPages(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseLimit(std::istringstream &lineStream, t_serverConfig &serverConfig);
		void	_parseSimpleParams(std::istringstream &lineStream, std::string &param);

		// === Parse server URL configuration functions ===

		void	_parseUrl(t_serverConfig &serverConfig, t_linesIterator &it);
		t_route	_parseUrlConfigs(t_linesIterator &it);
		std::string	_getRouteName(t_linesIterator &it) const;
		void	_parseMethods(std::istringstream &lineStream, t_route &route);
		void	_parseDirList(std::istringstream &lineStream, t_route &route);
		void	_parseCgi(std::istringstream &lineStream, std::map<std::string, std::string> &cgi);

		// === Parse util functions ===

		bool	_isLineInvalid(std::string const &line);
		void	_jumpInvalidLines(t_linesIterator &it);
};

#endif /* PARSER_HPP */
