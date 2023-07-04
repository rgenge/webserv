
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
# include "ServerConfig.hpp"
# include "webserv.hpp"
# include "CgiHandler.hpp"
# include <string>
# include <cstring>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sstream>
# include <ctime>
# include <sys/dir.h>
#include <iostream>
#include <string>

class Response
{
	private:
		std::string							_response;
		std::string							_body;
		std::map <std::string, std::string>	_res_map;
		std::string							_full_path;
		std::string							_dir_path;
		std::map <std::string, std::string>	&_res_param;
		std::map <std::string, std::string>	&_req_parsed;
		t_serverConfig						&_serverConfig;
		std::string							&_actual_root;
		int									_index_flag;
		int									_path_flag;
		std::string							_path_location;
		ServerConfig						&_configs;
		std::string							&_url_path;

		// POST
		std::string 						_boundary;
		std::string							_fileType;
		std::string							_postBodyStr;
		std::map <std::string, std::string>	_vars;
		std::map <std::string, std::string>	_boundaryHeaders;
		std::map <std::string, std::string>	_postHeaders;
		std::vector<unsigned char>			_postBody;
		std::vector<unsigned char>			&_requestData;

		// POST
		void		_methodPost(t_serverConfig &serverConfig);
		void		_parseRequestData(void);
		void		_setPostBodyStr(void);
		void		_parseChunk(void);
		void		_parseUrlEncodedParams(void);
		void		_removeBreakLinesAndCR(std::string &str);
		void		_replaceHexPercentWithAscii(std::string &params);
		void		_parseMultipartFormData(t_serverConfig &serverConfig);
		void		_parseTextPlain(void);
		void		_setBoundary(void);
		void		_removeHeaderSpaces(std::string &multipart);
		void		_setHeaders(void);
		void		_processBoundaryHeaders(t_serverConfig &serverConfig);
		void		_handleBoundaryPart(t_serverConfig &serverConfig);
		void		_handleImputFile(std::string &contentDisposition, t_serverConfig &serverConfig);
		void		_setPostBodyVector(void);
		std::string	_handleLastSlash(std::string &Route);
		std::string	_getUploadDir(t_serverConfig &serverConfig);
		std::string	_originalFileName(std::string &contentDisposition);
		std::string	_generateFileName(std::string const &originalFileName);
		std::string	_setStringHeaders(void);
		int			_findSequence(std::vector<unsigned char> &vector, std::string const sequence);

	public:
		Response(std::map <std::string, std::string>& _res_param_, std::map<std::string, std::string>& _req_parsed_,
		t_serverConfig&_serverConfig_, std::string& _actual_root_,ServerConfig & _configs_, std::string& _url_path_, std::vector<unsigned char> &requestData);
		~Response();
		void		init (int _flag);
		void		methodGet(std::map <std::string, std::string> _req_parsed,
			std::map <std::string, std::string> _res_param);
		void		printHeader(std::string status_code, std::string ok_ko, std::string http_version);
		void		autoIndex(std::map <std::string, std::string> _res_param);
		void		methodDelete(std::map <std::string, std::string>
			_req_parsed, std::map <std::string, std::string> _res_param);
		bool		dirCheck(std::string dir);
		std::string sizetToString(std::string text);
		std::string	getResponse();
		std::string	getType();
		std::string	getBody();

};
#endif
