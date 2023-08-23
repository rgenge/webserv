#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <cstring>
# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <fstream>
# include <map>
# include <vector>
# include <unistd.h>
# include <sstream>

class Request
{
	private:
		std::vector<unsigned char>			&_requestData;
		std::string							_body;
		std::string							_strBody;
		std::map <std::string, std::string>	_headers;

		void		_parse(void);
		void		_parseChunk(void);
		size_t		_heximalConverter(std::string input);
		void		_setStrBody(void);
		std::string	_setStringHeaders(void);

	public:
		Request(std::vector<unsigned char> &requestData);
		~Request();
		void								checkRequest();
		void								get_query();
		std::map<std::string, std::string>	&getMap();
		std::string							&getStrBody();
		std::vector<unsigned char>			&getVectorBody();
};

#endif
