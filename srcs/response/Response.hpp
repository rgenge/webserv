#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
# include "ServerConfig.hpp"
# include "ErrorResponse.hpp"
# include "webserv.hpp"
# include "CgiHandler.hpp"
# include <cstring>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sstream>
# include <ctime>
# include <sys/dir.h>
# include <iostream>
# include <string>
# include <sys/stat.h>
# include <cstddef>

class Response
{
	private:
		std::string							_response;
		std::string							_body;
		std::map <std::string, std::string>	_res_map;
		std::string							_full_path;
		std::string							_dir_path;
		std::map <std::string, std::string>	&_req_parsed;
		t_serverConfig						&_serverConfig;
		ServerConfig						_configs;
		std::string							&_url_path;

		// POST
		std::string 						_boundary;
		std::string							_fileType;
		std::string							&_strBody;
		std::string							_phpSuffix;
		std::string							_pythonSuffix;
		std::string							_suffix;
		std::map <std::string, std::string>	_vars;
		std::vector<unsigned char>			&_vectorBody;
		std::string							&_actual_root;
		
		//GET
		void		methodGet(std::map <std::string, std::string> _req_parsed);
		void		printHeader(std::string status_code, std::string message,
			std::string http_version);
		void		autoIndex();
		void		methodDelete(std::map <std::string, std::string>_req_parsed);
		int			dirCheck(std::string dir);
		std::string	sizetToString(std::string text);
		bool		checkRequest();
		void		getCookie();

		// POST
		void		_methodPost(void);
		void		_handleMultipart(std::string &fileName);
		void		_isNotCGI(void);
		void		_checkCgiRequest(void);
		void		_checkUploadPath(void);
		void		_parseUrlEncodedParams(void);
		void		_removeBreakLinesAndCR(std::string &str);
		void		_replaceHexPercentWithAscii(std::string &params);
		void		_replacePlusWithSpace(std::string &params);
		void		_parseTextPlain(void);
		void		_setBoundary(void);
		void		_sendDataToHandlerCGI(void);
		std::string	_originalFileName(std::string &contentDisposition);
		std::string	_generateFileName(std::string const &type, std::string const &originalFileName);
		size_t		_findSequence(std::vector<unsigned char> &vector, std::string const sequence);

	public:
		Response(std::map<std::string, std::string>& _req_parsed_,
		t_serverConfig&_serverConfig__, std::string& _url_path_, std::string &strBody,
		std::vector<unsigned char> &vectorBody, std::string& _actual_root_);
		~Response();
		void		init ();
		std::string	getResponse();
		std::string	getType();
		std::string	getBody();
		bool		headerCheck();
};
#endif
