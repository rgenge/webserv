
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

class Response
{
	private:
		std::string		_response;
		std::string		_body;
		std::map <std::string, std::string>	_res_map;
		std::string		_full_path;


	public:
		Response();
		Response(std::string res_input);
		~Response();
		void parse (std::string res_input);
		void init (std::map <std::string, std::string> map_inpu, std::map <std::string, std::string> server_conf);
		std::string getResponse();
		void method_get(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
		void print_header(std::string status_code, std::string ok_ko);
		std::string get_type();
		std::string get_body();

};

#endif
