
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
# include <sys/dir.h>

class Response
{
	private:
		std::string							_response;
		std::string							_body;
		std::map <std::string, std::string>	_res_map;
		std::string							_full_path;
		std::string							_dir_path;
		std::string							_textPlain;
		std::map <std::string, std::string>	_decodedParams;


	public:
		Response();
		Response(std::string res_input);
		~Response();
		void parse (std::string res_input);
		void init (std::map <std::string, std::string> map_inpu, std::map <std::string, std::string> server_conf);
		void method_get(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
		void print_header(std::string status_code, std::string ok_ko);
		void auto_index(std::map<std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
		void methodPost(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf);
		void parseUrlEncodedParams(std::string params);
		void removeBreakLines(std::string &str);
		void replaceHexPercentWithAscii(std::string &params);
		void parseMultipartFormData(std::string &multipart);
		void parseTextPlain(std::string &textPlain);
		std::string getResponse();
		std::string get_type();
		std::string get_body();

};

#endif
