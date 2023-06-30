#include "Response.hpp"

Response::Response(std::map <std::string, std::string>& _res_param_, std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_, std::string& _actual_root_, ServerConfig & _configs_, std::string&_url_path_,
std::vector<unsigned char> &requestData)
:_res_param(_res_param_), _req_parsed(_req_parsed_), _serverConfig
(_serverConfig_), _actual_root(_actual_root_), _configs(_configs_), _url_path(_url_path_), _requestData(requestData)
{
}

/*Salva a resposta e imprimi no terminal*/
void	Response::printHeader(std::string status_code, std::string ok_ko, std::string http_version)
{
	_response.append(http_version + status_code + " " + ok_ko +  "\r\n");
	for (std::map<std::string, std::string>::iterator i = _res_map.begin();
		i != _res_map.end(); i++)
		_response.append((*i).first + ": " + (*i).second + "\r\n");
	_response.append("\r\n");
}

/*Procura o ultimo "." do path e pega a extensao a partir dele*/
std::string	Response::getType()
{
	std::string		type;
	if(_full_path.find(".") < _full_path.length())
	{
		std::size_t last_dot = _full_path.find_last_of(".");
		type = _full_path.substr(last_dot, _full_path.size() - last_dot);
		if ((type==".css")) return "text/css";
		if (type==".csv") return "text/csv";
		if (type ==".htm") return "text/html";
		if (type ==".shtml") return "text/html";
		if (type ==".js") return "application/javascript";
		if (type ==".json") return "application/json";
		if (type ==".gif") return "image/gif";
		if (type ==".jpeg") return "image/jpeg";
		if (type ==".jpg") return "image/jpg";
		if (type ==".png") return "image/png";
		if (type ==".svg") return "image/svg+xml";
		if (type ==".ico") return "image/x-icon|";
	}
	return ("text/html");
}

void	Response::autoIndex( std::map <std::string, std::string> _res_param)
{
	DIR *dh;
    struct dirent *contents;
    std::string directory = _dir_path;
    std::fstream f;
    std::string _content;
	if (!(dh = opendir(directory.c_str())))
	{
		std::cout << "error message e saiu" << std::endl;
	}
	else
	{
		_body += ("<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
			"<title>webserv</title></head><body><h1>Index of " + _res_param
			["Root"] + "</h1>\n");
			while ((contents = readdir(dh)) != NULL)
			{
				if (!strcmp(contents->d_name, ".") || !strcmp(contents->d_name,
					".."))
					continue;
				else
					_body += "<a href=\"" + _req_parsed["Path"] +"/" + std::string(contents->d_name);
				_body += (contents->d_type == DT_DIR ? "" : "") + (std::string)
					"\">";
				_body += (std::string)(contents->d_name) + (contents->d_type ==
					DT_DIR ? "/" : "") + "</a><br>";
			}
			closedir(dh);
			_body += "</div></body></html>";
	}
}

/*
void	Response::printError(std::string codigo)
{
	std::cerr << codigo << std::endl;
}
*/

std::string Response::sizetToString(std::string text)
{
	std::stringstream sizet_num;
	sizet_num <<  text.size();
	std::string string_num = sizet_num.str();
	return (string_num);
}
void	Response::methodGet(std::map <std::string, std::string> _req_parsed,
	std::map <std::string, std::string> _res_param)
{
	std::string check_url;
	check_url = _req_parsed["Path"].substr(0, _url_path.size());
	std::cout << "check:" << check_url << std::endl;
	/*Limpa o caminho e tira o url pra adicionar ao root*/
	std::string		clean_path = _req_parsed["Path"];
	std::string::size_type i = clean_path.find(_url_path);
	if (i != std::string::npos && _url_path != "/")
		clean_path.erase(i, _url_path.length());
	if (check_url == _url_path)
	{
		_full_path = _configs.getRoot() + clean_path;
		_dir_path = _configs.getRoot() + clean_path;
	}
//	else if ((_req_parsed["Path"] == "/"))
//	{
//		_full_path = _configs.getRoot() + _req_parsed["Path"] + "/" + _configs.getIndex();
//		_dir_path = _configs.getRoot() + _req_parsed["Path"];
//	}
	else
	{
		_full_path = _configs.getRoot() + clean_path;
		_dir_path = _configs.getRoot() + clean_path;
	}
	if(_full_path.find("//") != std::string::npos)
		_full_path.replace(_full_path.find("//"), 2, "/");
	if (access (((_full_path + + "/" + _configs.getIndex()).c_str()), F_OK) != -1)
		_full_path = _full_path + "/" +  _configs.getIndex();
	if (_configs.getRedirect() != "")
		_full_path = _configs.getRedirect();
	/*CGI funciona mas sem verificar input do server*/
	if (_full_path.find(".php") != std::string::npos)
	{
		CgiHandler	cgi_init;
		std::string	cgi_body;
		_body = cgi_init.cgiHandler(_full_path);
		return;
	}
	std::cout <<"FULL \t:" << _full_path <<std::endl;
	if (access ((const char *)_full_path.c_str(), F_OK) != -1)
	{
		std::ifstream	page;
		std::string		buffer;
		std::string		line;
		/*Abre o arquivo e salva os dados da pagina no buffer*/
		page.open(_full_path.c_str(), std::ifstream::in);
		while (std::getline(page, line)) {
			buffer += line + "\n";
		}
		if (_configs.getDirList() == 0 || !dirCheck(_full_path))
			_body = buffer;
		if (_configs.getDirList() == 1 && dirCheck(_full_path))
			autoIndex(_res_param);
		/*Converte o Content-length de size_t pra string e adiciona no map*/
		std::string string_len = sizetToString(buffer);
		if (_configs.getDirList() == 0 || !dirCheck(_full_path))
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		else if (_configs.getDirList() == 1 && dirCheck(_full_path))
		{
			std::string string_len = sizetToString(_body);
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		}
		/*Verifica o Content-type do arquivo*/
		_res_map["Content-type"] = getType();
		/*Verifica o Content-Length e imprimi erro caso necessário*/
		int content = atoi(_res_map["Content-Length"].c_str());
		if((content) > _configs.getBodySizeLimit())
			std::cout << "error 414 bodySize Limite excedido";
		printHeader ("200", "OK", _req_parsed["Version"]);
		page.close();
	}
	else
		std::cout << "Some error ";
}

void	Response::methodDelete(std::map <std::string, std::string> _req_parsed,
	std::map <std::string, std::string> _res_param)
{
	if (_req_parsed["Path"] == "/")
	{
		_full_path = _res_param["Root"] + _req_parsed["Path"] + _res_param
			["Index"];
		_dir_path = _res_param["Root"] + _req_parsed["Path"];
	}
	else
	{
		_full_path = _res_param["Root"] + _req_parsed["Path"];
		_dir_path = _res_param["Root"] + _req_parsed["Path"];
	}
	FILE *file = fopen(_full_path.c_str(), "r");
	if(!file)
	{
			std::cout << "Error 404";
	}
	else
	{
		std::remove (_full_path.c_str());
		printHeader ("200", "OK", _req_parsed["Version"]);
	}
}

/*Checa sé não é diretório para ligar autoindex*/
bool	Response::dirCheck(std::string dir)
{
	struct stat buf;
	lstat(dir.c_str(), &buf);
	FILE *check_fp = fopen(dir.c_str(), "r");
	if (!check_fp)
	{
		std::cerr << "Error 404 dirCheck";
		return (1);
	}
	else
	{
		fclose(check_fp);
		if (S_ISDIR(buf.st_mode))
			return 1;
		/*não é diretório*/
		return 0;
	}
}

std::string	intToString (int num)
{
	std::stringstream temp;
	temp<<num;
	return temp.str();
}

void Response::_parseChunk(std::string &body)
{
	(void)body;
	return ;
}

void Response::_removeBreakLinesAndCR(std::string &str)
{
	std::string	temp;
	for (int i = 0; str[i]; i++)
	{
		if (str[i] != '\n' && str[i] != '\r')
        	temp += str[i];
	}
	str = temp;
	return ;
}

void Response::_replaceHexPercentWithAscii(std::string &params)
{
	int	pos = 0;
	while (true)
	{
		size_t	percentPos = params.find('%', pos);
		if (percentPos == std::string::npos)
            break ;
		else
		{
			if ((params.size() - percentPos) > 2)
			{
				std::string hexStr;
				hexStr = params.substr(percentPos + 1, 2);

				int	value;
				std::istringstream	iss(hexStr);
				iss >> std::hex >> value;
				if (iss.fail())
					throw std::runtime_error("failed to convert hex to ASCII");

				std::string asciiChar;
				asciiChar.push_back(static_cast<char>(value));
				params.replace(percentPos, 3, asciiChar);
			}
			else
				throw std::runtime_error("invalid parameters format");
		}
	}
	return ;
}

void Response::_parseUrlEncodedParams(std::string params)
{
	size_t separatorPos = params.find('=');
	if (separatorPos == std::string::npos)
		throw std::runtime_error("invalid application/x-www-form-urlencoded format");
	_removeBreakLinesAndCR (params);
	_replaceHexPercentWithAscii(params);
	size_t pos = 0;
	while (true)
	{
		separatorPos = params.find('=', pos);
		if (separatorPos == std::string::npos)
            break ;
		std::string	key = params.substr(pos, separatorPos - pos);
		params.erase(pos, separatorPos + 1);

		size_t ampersandPos = params.find('&', pos);
		if (ampersandPos == std::string::npos && params.size() == 0)
            break ;
		else if (ampersandPos == std::string::npos)
			ampersandPos = params.size();
		std::string	value = params.substr(pos, ampersandPos - pos);
		params.erase(pos, ampersandPos + 1);

		this->_decodedParams[key] = value;
	}
	// std::map<std::string, std::string>::iterator it;
	// for(it = this->_decodedParams.begin(); it != this->_decodedParams.end(); it++)
	// 	std::cout << it->first << "=" << it->second << std::endl;
	return ;
}

void	Response::_parseTextPlain(std::string &textPlain)
{
	_removeBreakLinesAndCR(textPlain);
	this->_textPlain = textPlain;
	return ;
}

void	Response::_setBoundary(std::string &contentType)
{
	size_t boundaryPos;
	if ((boundaryPos = contentType.find("---")) != std::string::npos)
		this->_boundary = contentType.substr(boundaryPos, contentType.size() - boundaryPos);
	else
		throw std::runtime_error("boundary not found");
	return ;
}

void	Response::_removeHeaderSpaces(std::string &multipart)
{
	size_t		endHeaderPos, i = 0;
	std::string	tempMultipart;

	if ((endHeaderPos = multipart.find("\r\n\r\n")) != std::string::npos)
	{
		while (i < endHeaderPos)
		{
			if (multipart[i] != ' ')
				tempMultipart += multipart[i];
			i++;
		}
		while (i < multipart.size())
		{
			tempMultipart += multipart[i];
			i++;
		}
		multipart = tempMultipart;
	}
	else
		throw std::runtime_error("invalid multipart/formdata 'endHeaderPos'");

	return ;
}

void	Response::_setHeaders(std::string &multipart)
{
	size_t		boundaryPos, colonPos, argsPos;
	std::string	headerKey, headerValue;

	if ((boundaryPos = multipart.find(this->_boundary)) != std::string::npos)
		multipart.erase(0, boundaryPos + this->_boundary.size() + 2);
	else
	{
		std::cout << "multipart error :\n" << multipart << std::endl;
		std::cout << "\nboundary: " << this->_boundary << std::endl;
		throw std::runtime_error("invalid multipart/formdata 'boundaryPos'");
	}
	_removeHeaderSpaces(multipart);
	while (true)
	{
		if ((colonPos = multipart.find(':')) != std::string::npos)
		{
			headerKey = multipart.substr(0, colonPos);
			multipart.erase(0, headerKey.size() + 1);
			if ((argsPos = multipart.find("\r\n")) != std::string::npos)
			{
				headerValue = multipart.substr(0, argsPos);
				multipart.erase(0, headerValue.size());
			}
			else
				throw std::runtime_error("invalid multipart/formdata 'argsPos'");

			this->_multipartHeaders[headerKey] = headerValue;
			if ((multipart.size() >= 4) && (multipart.substr(0, 4) == "\r\n\r\n"))
			{
				multipart.erase(0, 4);
				return ;
			}
			else
				multipart.erase(0, 2);
		}	
		else
			throw std::runtime_error("invalid multipart/formdata 'colonPos'");
	}
	return ;
}

std::string	Response::_originalFileName(std::string &contentDisposition)
{
	std::string	originalFileName = "_";
	std::string	fieldFileName = "filename=\"";

	size_t	fileNamePos;
	if ((fileNamePos = contentDisposition.find(fieldFileName)) != std::string::npos)
	{
		int	i = fileNamePos + fieldFileName.size();
		while ((contentDisposition[i]) && (contentDisposition[i] != '\"'))
			originalFileName += contentDisposition[i++];
		// std::cout << originalFileName << std::endl;
	}
	else
		throw std::runtime_error("invalid multipart/formdata '_fileExtension'");
	return (originalFileName);
}

std::string	Response::_generateFileName(std::string const &originalFileName)
{
	static int	sequenceNumber = 0;
	std::time_t	currentTime;
	std::stringstream	fileName;

	currentTime = std::time(0);
	fileName << "file_" << currentTime << sequenceNumber << originalFileName;
	sequenceNumber++;
	return (fileName.str());
}

std::string	Response::_handleLastSlash(std::string &Route)
{
	std::string	alternativeRoute;

	if (Route[Route.size() - 1] != '/')
		alternativeRoute = Route + '/';
	else
		alternativeRoute = Route.substr(0, Route.size() - 1);
	std::cout << "alternative: " << alternativeRoute << std::endl;
	return (alternativeRoute);
}

std::string	Response::_getUploadDir(t_serverConfig &serverConfig)
{
	std::map<std::string, t_route>::iterator it;

	if (this->_postHeaders["Path"] == "/")
		throw std::runtime_error("error 405 '_getDir'");
	else
	{
		if (((it = serverConfig.routes.find(this->_postHeaders["Path"])) != serverConfig.routes.end())
		|| ((it = serverConfig.routes.find(_handleLastSlash(this->_postHeaders["Path"]))) != serverConfig.routes.end()))
		{
			// std::cout << "Encontrou a rota!" << std::endl;
			;
		}
		else
			throw std::runtime_error("error 405 '_getDir'");
	}
	return ("");
}

void	Response::_handleImputFile(std::string &contentDisposition, std::string &multipart, t_serverConfig &serverConfig)
{
	std::ofstream	file;
	std::string		originalFileName;

	_getUploadDir(serverConfig);
	originalFileName = _originalFileName(contentDisposition);
	file.open(_generateFileName(originalFileName).c_str(), std::ios::out);
	if (!file.is_open())
		throw std::runtime_error("open file error '_handleImputFile'");
	file << multipart;
	file.close();
	return ;
}

void	Response::_processBoundaryHeaders(std::string &multipart, t_serverConfig &serverConfig)
{
	if (this->_multipartHeaders["Content-Disposition"].find("form-data;") != std::string::npos)
	{
		if (this->_multipartHeaders["Content-Disposition"].find("filename=") != std::string::npos)
			_handleImputFile(this->_multipartHeaders["Content-Disposition"], multipart, serverConfig);
		else
			throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	}
	else
		throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	return ;
}

void	Response::_handleBoundaryPart(std::string &multipart, t_serverConfig &serverConfig)
{
	_setHeaders(multipart);
	// std::map<std::string, std::string>::iterator it;
	// for(it = this->_multipartHeaders.begin(); it != this->_multipartHeaders.end(); it++)
	// 	std::cout << "key: " << it->first << " | value: " << it->second << std::endl;
	_processBoundaryHeaders(multipart, serverConfig);
}

void	Response::_parseMultipartFormData(std::string &contentType, std::string &multipart, t_serverConfig &serverConfig)
{
	_setBoundary(contentType);
	_handleBoundaryPart(multipart, serverConfig);
	return ;
}

void	Response::_methodPost(std::map <std::string, std::string> map_input,
	t_serverConfig &serverConfig)
{
	if (map_input["Transfer-Encoding"] == "chunked")
		_parseChunk(map_input["ChunkBody"]);
	if (map_input["Content-Type"] == "application/x-www-form-urlencoded")
		_parseUrlEncodedParams(map_input["ChunkBody"]);
	else if (map_input["Content-Type"] == "text/plain")
		_parseTextPlain(map_input["ChunkBody"]);
	else if (map_input["Content-Type"].find("multipart/form-data") != std::string::npos)
		_parseMultipartFormData(map_input["Content-Type"], map_input["ChunkBody"], serverConfig);
	else
		std::cout << map_input["Content-Type"] << std::endl;
	return ;
}

int	Response::_findSequence(std::string const sequence)
{
	size_t i = 0;
	size_t j = 0;
	while ((i < this->_requestData.size()) && (j < sequence.size()))
	{
		if (this->_requestData[i] == sequence[j])
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
		return (0);
	return (i);
}

std::string	Response::_setStringHeaders(void)
{
	std::string	postHeaders;

	if (this->_requestData.size() > 3)
	{
		int headersEndPos = _findSequence("\r\n\r\n");
		if (headersEndPos > 0)
		{
			for (int i = 0; i < headersEndPos; i++)
			{
				postHeaders += this->_requestData[i];
				this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin());
			}
			std::cout << postHeaders << std::endl;\
		}
		else
			throw std::runtime_error("invalid request format '_setPostHeaders'");
	}
	else
		throw std::runtime_error("invalid request format '_setPostHeaders'");
	return (postHeaders);
}

void	Response::_parseRequestData(void)
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
				this->_postHeaders.insert(make_pair(sub, token));
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
					this->_postHeaders.insert(make_pair(head1, sub));
					counter++;
				}
				else if (counter == 2)
				{
					this->_postHeaders.insert(make_pair(head2, sub));
					this->_postHeaders.insert(make_pair(head3, token));
					counter++;
				}
			}
			s.erase(0, pos + delim.length());
		}
	}
	return ;
}


void	Response::init(int _flag)
{
 	if (_flag == 1)
 	{
// 	 	_actual_root = _serverConfig.root;
	 	_flag = 1;
// 		_url_path = _req_parsed["Path"];
 	}
	// std::cout <<"Root:"<< _configs.getRoot() << std::endl;
	// std::cout <<"Auxindex:"<< _configs.getDirList() << std::endl;
	// std::cout <<"Indexx:"<< _configs.getIndex()<< std::endl;
	// std::cout <<"LimitSize:"<< _configs.getBodySizeLimit()<< std::endl;
	// std::cout <<"Redirect:"<< _configs.getRedirect() << std::endl;
	// std::cout <<"UploadPath:"<< _configs.getUploadPath() << std::endl;
	// std::cout <<"Req_parsed_path"<< _req_parsed["Path"]<< std::endl;
	// std::cout <<"Url path :"<<  _url_path<< std::endl;
	if (_req_parsed["Method"] == "GET")
		methodGet(_req_parsed, _res_param);
	if (_req_parsed["Method"] == "DELETE")
		methodDelete(_req_parsed, _res_param);
	_parseRequestData();
	std::map<std::string, std::string>::iterator it;
	for (it = this->_postHeaders.begin(); it != this->_postHeaders.end(); it++)
		std::cout << it->first << "=" << it->second << std::endl;
	if (this->_postHeaders["Method"] == "POST")
		_methodPost(_req_parsed, _serverConfig);
	throw std::runtime_error("EXIT");
}

Response::~Response()
{
}

std::string Response::getResponse()
{
	return _response;
}

std::string Response::getBody()
{
	return _body;
}
