#include "Request.hpp"

/*Quando chama o construtor com req ele já inicializa e chama funções*/
Request::Request(std::vector<unsigned char> &requestData): _requestData(requestData)
{
	_parse();
}

size_t	_findSequence(std::vector<unsigned char> &vector, std::string const sequence)
{
	size_t i = 0;
	size_t j = 0;
	while ((i < vector.size()) && (j < sequence.size()))
	{
		if (vector[i] == sequence[j])
		{
			i++;
			j++;
		}
		else
		{
			i -= j;
			j = 0;
			i++;
		}
	}
	if (j < sequence.size())
		return (std::string::npos);
	return (i - j);
}

std::string	Request::_setStringHeaders(void)
{
	size_t		npos;
	std::string	headers;
	size_t		endHeader = 4;

	if (this->_requestData.size() > 3)
	{
		if ((npos = _findSequence(this->_requestData, "\r\n\r\n")) != std::string::npos)
		{
			for (size_t i = 0; i < (npos + endHeader); i++)
			{
				headers += this->_requestData[0];
				this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin() + 1);
			}
		}
		else
			this->_headers["Version"] = "Bad Request";
	}
	else
		this->_headers["Version"] = "Bad Request";
	return (headers);
}

void	Request::_setStrBody(void)
{
	if (this->_headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		this->_strBody = "";
		return ;
	}
	for (size_t i = 0; i < this->_requestData.size(); i++)
		this->_strBody += this->_requestData[i];
	return ;
}

void Request::get_query()
{
	std::string path = this->_headers["Path"];
	size_t i = path.find_first_of("?", 0);
	if (i == std::string::npos)
		return ;
	this->_headers.insert(make_pair("Query", path.substr(i + 1, path.size() - i)));
	this->_headers["Path"] = path.substr(0, i);
}

void	Request::_parse(void)
{
	std::string		s;
	std::string		sub;
	std::string		sub2;
	std::string		token;
	std::string		delim = "\r\n";
	std::string		head1 = "Method";
	std::string		head2 = "Path";
	std::string		head3 = "Version";
	size_t			pos = 0;
	int				counter = 1;

	s = _setStringHeaders();
	while ((pos = s.find(delim)) != std::string::npos)
	{
		if (s[0] == '\r' && s[1] == '\n')
			break;
		token = s.substr(0, pos);
		size_t found = token.find("HTTP/");
		if (found == std::string::npos)
		{
			size_t pos2 = 0;
			std::string delim2 = ": ";
			if (token.find(" :") != std::string::npos)
			{
				this->_headers["Version"] = "Bad Request";
				return;
			}
			while((pos2 = token.find(delim2)) != std::string ::npos)
			{
				sub = token.substr(0, pos2);
				token.erase(0, pos2 + delim2.length());
				this->_headers.insert(make_pair(sub, token));
			}
			s.erase(0, pos + delim.length());
		}
		else
		{
			size_t pos2 = 0;
			std::string delim3 = " ";
			while((pos2 = token.find(delim3)) != std::string ::npos)
			{
				sub = token.substr(0, pos2);
				token.erase(0, pos2 + delim3.length());;
				if (counter == 1)
				{
					this->_headers.insert(make_pair(head1, sub));
					counter++;
				}
				else if (counter == 2)
				{
					this->_headers.insert(make_pair(head2, sub));
					this->_headers.insert(make_pair(head3, token));
					counter++;
				}
			}
			s.erase(0, pos + delim.length());
		}
	}
	get_query();
	_setStrBody();
	return ;
}

Request::~Request()
{
}

std::map<std::string, std::string> &Request::getMap()
{
	return (this->_headers);
}

std::string &Request::getStrBody(void)
{
	return (this->_strBody);
}

std::vector<unsigned char> &Request::getVectorBody(void)
{
	return (this->_requestData);
}
