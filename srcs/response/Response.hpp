
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
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
		// POST
		std::string							_textPlain;
		std::string 						_boundary;
		std::string							_fileType;
		std::map <std::string, std::string>	_decodedParams;
		std::map <std::string, std::string>	_multipartHeaders;
		std::map <std::string, std::string>	_mapImput;

		// POST
		void		_methodPost(std::map <std::string, std::string> map_input, t_serverConfig &serverConfig);
		void		_parseChunk(std::string &body);
		void		_parseUrlEncodedParams(std::string params);
		void		_removeBreakLinesAndCR(std::string &str);
		void		_replaceHexPercentWithAscii(std::string &params);
		void		_parseMultipartFormData(std::string &contentType, std::string &multipart, t_serverConfig &serverConfig);
		void		_parseTextPlain(std::string &textPlain);
		void		_setBoundary(std::string &contentType);
		void		_removeHeaderSpaces(std::string &multipart);
		void		_setHeaders(std::string &multipart);
		void		_processBoundaryHeaders(std::string &multipart, t_serverConfig &serverConfig);
		void		_handleBoundaryPart(std::string &multipart, t_serverConfig &serverConfig);
		void		_handleImputFile(std::string &contentDisposition, std::string &multipart, t_serverConfig &serverConfig);
		std::string	_handleLastSlash(std::string &Route);
		std::string	_getUploadDir(t_serverConfig &serverConfig);
		std::string	_originalFileName(std::string &contentDisposition);
		std::string	_generateFileName(std::string const &originalFileName);

	public:
		Response(std::map <std::string, std::string>& _res_param_, std::map<std::string, std::string>& _req_parsed_, t_serverConfig&_serverConfig_, std::string& _actual_root_);
		~Response();
		void		init (int _flag);
		void		methodGet(std::map <std::string, std::string> _req_parsed,
			std::map <std::string, std::string> _res_param);
		void		printHeader(std::string status_code, std::string ok_ko, std::string http_version);
		void		autoIndex(std::map <std::string, std::string> _res_param);
		void		methodDelete(std::map <std::string, std::string>
			_req_parsed, std::map <std::string, std::string> _res_param);
		void		locationCheck();
		std::string	getResponse();
		std::string	getType();
		std::string	getBody();

};
#endif
