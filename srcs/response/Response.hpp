
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Server.hpp"
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

	public:
		Response(std::map <std::string, std::string>& _res_param_, std::map
		<std::string, std::string>& _req_parsed_, t_serverConfig&
		_serverConfig_);
		~Response();
		void		init ();
		void		method_get(std::map <std::string, std::string> _req_parsed,
			std::map <std::string, std::string> _res_param);
		void		print_header(std::string status_code, std::string ok_ko);
		void		auto_index(std::map <std::string, std::string> _res_param);
		void		method_delete(std::map <std::string, std::string>
			_req_parsed, std::map <std::string, std::string> _res_param);
		void		locationCheck();
		std::string	getResponse();
		std::string	get_type();
		std::string	get_body();

};
#endif
