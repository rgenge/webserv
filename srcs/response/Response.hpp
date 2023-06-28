
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

	public:
		Response(std::map <std::string, std::string>& _res_param_, std::map
		<std::string, std::string>& _req_parsed_, t_serverConfig&
		_serverConfig_, std::string& _actual_root_);
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
