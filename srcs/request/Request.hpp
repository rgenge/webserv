
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <cstring>
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <fstream>
# include <map>
# include <unistd.h>
# include <sstream>

class Request
{
	private:
		std::string							_request;
		std::map <std::string, std::string>	_req_map;
		std::string							_body;
		std::string							_error_type;
	public:
		Request(std::string req_input);
		~Request();
		void								parse(std::string req_input);
		void								parse_chunk(std::string request);
		void								check_request();
		std::map <std::string, std::string>	&getmap();
		std::string							getbody();
		std::string							errortype();
		std::string							getrequest();
};

#endif
