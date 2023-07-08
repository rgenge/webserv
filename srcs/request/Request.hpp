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
		std::map <std::string, std::string>	_req_map;
		std::string							_body;
	public:
		Request(std::string req_input);
		~Request();
		void								parse(std::string req_input);
		void								parseChunk(std::string request);
		void								checkRequest();
		size_t								heximalConverter(std::string input);
		std::map <std::string, std::string>	&getMap();
		std::string							getBody();
};

#endif
