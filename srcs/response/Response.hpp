
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <cstring>
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <fstream>
# include <map>
# include <unistd.h>
# include <sstream>
# include <ctime>
# include "webserv.hpp"

class Response
{
	private:
		std::string							_response;
		std::string							_body;
		std::map <std::string, std::string>	_res_map;
		std::string							_full_path;
		std::string							_dir_path;
		
		// POST
		std::string							_textPlain;
		std::string 						_boundary;
		std::string							_fileType;
		std::map <std::string, std::string>	_decodedParams;
		std::map <std::string, std::string>	_multipartHeaders;

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
		std::string	_getDir(t_serverConfig &serverConfig);
		std::string	_originalFileName(std::string &contentDisposition);
		std::string	_generateFileName(std::string const &originalFileName);

	public:
		Response();
		Response(std::string res_input);
		~Response();
		std::string getResponse();
		std::string get_type();
		std::string get_body();
		void parse (std::string res_input);
		void init (std::map <std::string, std::string> map_inpu, std::map <std::string, std::string> server_conf, t_serverConfig &serverConfig);
		void method_get(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
		void print_header(std::string status_code, std::string ok_ko);
		void auto_index(std::map<std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
};

#endif
