#include "Request.hpp"

/*Quando chama o construtor com req ele já inicializa e chama funções*/
Request::Request(std::string req_input)
{
	parse(req_input);
	if (_req_map.count("Transfer-Encoding") > 0 && _req_map["Transfer-Encoding"]
		== "chunked")
		parseChunk(_req_map["ChunkBody"]);
}
/*Faz o parseamento do input do request e separa em um std::map  */
void Request::parse(std::string req_input)
{
	std::string		s = req_input;
	std::string		delim = "\r\n";
	size_t			pos = 0;
	std::string		token;
	std::string		sub;
	std::string		sub2;
	std::string		head1 = "Method";
	std::string		head2 = "Path";
	std::string		head3 = "Version";
	std::string		head4 = "Body";
	std::string		head5 = "ChunkBody";
	int counter = 1;

	while ((pos = s.find(delim)) != std::string::npos)
	{
		if (s[0] == '\r' && s[1] == '\n')
		{
			_req_map.insert(make_pair(head5, s));
			s = "";
		}
		token = s.substr(0, pos);
		size_t found = token.find("HTTP/");
		if (found == std::string::npos)
		{
			std::string delim2 = ": ";
			size_t pos2 = 0;
			while((pos2 = token.find(delim2)) != std::string ::npos)
			{
				sub = token.substr(0, pos2);
				token.erase(0, pos2 + delim2.length());
				_req_map.insert(make_pair(sub, token));
			}
			s.erase(0, pos + delim.length());
		}
		else
		{
			std::string delim3 = " ";
			size_t pos2 = 0;
			while((pos2 = token.find(delim3)) != std::string ::npos)
			{
				sub = token.substr(0, pos2);
				token.erase(0, pos2 + delim3.length());;
				if (counter == 1)
				{
					_req_map.insert(make_pair(head1, sub));
					counter++;
				}
				else if (counter == 2)
				{
					_req_map.insert(make_pair(head2, sub));
					_req_map.insert(make_pair(head3, token));
					counter++;
				}
			}
			s.erase(0, pos + delim.length());
		}
	}/*Código abaixo apenas para testar o parser do request*/
	//  std::map<std::string, std::string>::iterator itr;
	//  for(itr=_req_map.begin();itr!=_req_map.end();itr++)
	//  {
	//  	std::cout<<itr->first<<"::"<<itr->second<<std::endl;
	//  }
}
/*Checa o tamanho do chunk e retorno o valor em decimal pra conversão*/
size_t Request::heximalConverter(std::string input)
{
	std::stringstream convert;
	size_t ret = 0;
	convert << std::hex << input;
	convert >> ret;
	return ret;
}
/*Faz o parseamento do chunk e gera um body tirando todos os \r\n */
void Request::parseChunk(std::string request)
{
	std::size_t size = 1;
	std::string  size_buf, buf;
	std::string line = request.substr(2, request.size());
	size_t i = 0;
	size_t j = 0;
	while (i < (line.size() - 2))
	{
		while (line[i] != '\r' && line[i + 1] != '\n')
			i++;
		size_buf = line.substr(j, i - j);
		size = heximalConverter(size_buf);
		if (size == 0)
			break;
		j = i + 2;
		buf = line.substr(j, size);
		_body = _body + buf;
		j = j + size + 2;
		i = j--;
	}
//	std::cout <<"print chunked bodu: "<< _body << std::endl;
}

Request::~Request()
{
}

std::map <std::string, std::string> &Request::getMap()
{
	return _req_map;
}

std::string Request::getBody(void)
{
	return _body;
}
