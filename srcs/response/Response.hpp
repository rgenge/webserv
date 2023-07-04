
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
# include "ServerConfig.hpp"
# include "webserv.hpp"
# include "CgiHandler.hpp"
# include <cstring>
# include <fstream>
# include <unistd.h>
# include <sstream>
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
		std::string							_path_location;
		ServerConfig						&_configs;
		std::string							&_url_path;
		int									_error_flag;
		void		methodGet(std::map <std::string, std::string> _req_parsed);
		void		printHeader(std::string status_code, std::string message,
			std::string http_version);
		void		autoIndex();
		void		methodDelete(std::map <std::string, std::string>
			_req_parsed);
		int		dirCheck(std::string dir);
		std::string	sizetToString(std::string text);
		void		printError(std::string codigo, std::string message);
		bool		checkRequest();

	public:
		void		init ();
		Response(std::map <std::string, std::string>& _res_param_, std::map
		<std::string, std::string>& _req_parsed_, t_serverConfig&
		_serverConfig_, ServerConfig & _configs_, std::string& _url_path_);
		~Response();
		std::string	getResponse();
		std::string	getType();
		std::string	getBody();

};
#endif
