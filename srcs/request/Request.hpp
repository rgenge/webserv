
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>

# include <cstring>
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <fstream>
# include <map>
//# include <sys/ioctl.h>
#include <unistd.h>
//# include <sys/socket.h>
#include <sstream>
//# include <netinet/in.h>

class Request
{
	private:
		std::string		_request;
		std::map <std::string, std::string>	_req_map;
		std::string		_body;
		std::string 	_error_type;
	public:
		Request(std::string req_input);
		~Request();
		std::string getrequest();
		void parse(std::string req_input);
		void parse_chunk(std::string request);
		void check_request();
		std::map <std::string, std::string> &getmap();
		std::string getbody();
		std::string errortype();
};

#endif
