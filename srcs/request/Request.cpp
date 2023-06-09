#include "Request.hpp"

/*Quando chama o construtor com req ele já inicializa e chama funções*/
Request::Request(std::vector<unsigned char> &requestData): _requestData(requestData)
{
	_parse();
	// if (this->_requestData.size() > 0 && this->_headers.count("Transfer-Encoding") > 0
	// && this->_headers["Transfer-Encoding"] == "chunked")
	// 	_parseChunk();
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
			throw std::runtime_error("invalid request format '_setStringheaders'");
	}
	else
		throw std::runtime_error("invalid request format '_setStringheaders'");
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
	_setStrBody();
	return ;
}

/*Checa o tamanho do chunk e retorno o valor em decimal pra conversão*/
// size_t Request::_heximalConverter(std::string input)
// {
// 	std::stringstream convert;
// 	size_t ret = 0;
// 	convert << std::hex << input;
// 	convert >> ret;
// 	return ret;
// }

// // /*Faz o parseamento do chunk e gera um body tirando todos os \r\n */
// void Request::_parseChunk(void)
// {
// 	std::size_t	size = 1;
// 	std::string	size_buf, buf;
// 	std::string	line = this->_strBody.substr(2, this->_strBody.size());
// 	size_t i = 0;
// 	size_t j = 0;
// 	while (i < (line.size() - 2))
// 	{
// 		while (line[i] != '\r' && line[i + 1] != '\n')
// 			i++;
// 		size_buf = line.substr(j, i - j);
// 		size = _heximalConverter(size_buf);
// 		if (size == 0)
// 			break;
// 		j = i + 2;
// 		buf = line.substr(j, size);
// 		_body = _body + buf;
// 		j = j + size + 2;
// 		i = j--;
// 	}
// }

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
