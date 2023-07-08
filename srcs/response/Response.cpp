#include "Response.hpp"

Response::Response(std::map <std::string, std::string>& _res_param_, std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_,
std::string&_url_path_,
std::vector<unsigned char> &requestData, std::string &_actual_root_) :_res_param(_res_param_),
_req_parsed(_req_parsed_), _serverConfig (_serverConfig_),
_url_path(_url_path_), _requestData(requestData), _actual_root(_actual_root_)
{
}

/*Salva a resposta e imprimi no terminal*/
void	Response::printHeader(std::string status_code, std::string message,
	std::string http_version)
{
	_response.append(http_version + " " + status_code + " " + message +  "\r\n");
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

void	Response::autoIndex()
{
	DIR *dh;
    struct dirent *contents;
    std::string directory = _dir_path;
    std::fstream f;
    std::string _content;
	if (!(dh = opendir(directory.c_str())))
	{
			//checar esse erro
			std::cerr << "Error 404? Path ñ existe ou s/ permissao"<< std::endl;
			printError("404", "Not Found");
			return ;
	}
	else
	{
		_body += ("<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
			"<title>webserv</title></head><body><h1>Index of " +
			_configs.getRoot() + "</h1>\n");
			while ((contents = readdir(dh)) != NULL)
			{

				if (!strcmp(contents->d_name, ".") || !strcmp(contents->d_name,
					".."))
					continue;
				else if (_configs.getRedirect() == "")
					_body += "<a href=\"" + _req_parsed["Path"] +"/" +
					std::string(contents->d_name);
				else
					_body += "<a href=\"" + _req_parsed["Path"];
				_body +=  "\">";
				_body += (std::string)(contents->d_name) + (contents->d_type ==
					DT_DIR ? "/" : "") + "</a><br>";
			}
			closedir(dh);
			_body += "</div></body></html>";
	}
}

void	Response::printError(std::string codigo, std::string message)
{
	_body = "";
	_body += ("<!DOCTYPE html><html><head><meta charset=\"UTF-8\" />"
		"<title>webserv</title></head><body><h1>Error " + codigo + "</h1>\n");
	_body += "</div></body></html>";
	_res_map["Content-type"] = getType();
	_res_map["Content-Length"] = "1000";
	_res_map["Connection"] = "close";
	printHeader (codigo, message, _req_parsed["Version"]);
}


std::string Response::sizetToString(std::string text)
{
	std::stringstream sizet_num;
	sizet_num <<  text.size();
	std::string string_num = sizet_num.str();
	return (string_num);
}

void	Response::methodGet(std::map <std::string, std::string> _req_parsed)
{
	if (_configs.getRedirect() != "")
	{
		_body = "";
		_res_map["Content-type"] = getType();
		std::string string_len = sizetToString(_body);
		_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		_res_map.insert(std::pair<std::string,std::string>
			("Location", _configs.getRedirect()));
		printHeader("307", "Temporary Redirect", _req_parsed["Version"]);
		return;
	}
	_full_path = _actual_root;
	_dir_path = _actual_root;
	/*limpa o path quando adicionar // */
	if(_full_path.find("//") != std::string::npos)
		_full_path.replace(_full_path.find("//"), 2, "/");
	/*Checa se o index existe caso exista acessa ele */
	if (access (((_full_path + "/" + _configs.getIndex()).c_str()), F_OK) !=
		-1)
	{
		if(_serverConfig.routes.find(_req_parsed["Path"]) != _serverConfig.routes.end() || _req_parsed["Path"] == "/")
			_full_path = _full_path + "/" +  _configs.getIndex();
		else
			_full_path = _full_path + "/" +  _req_parsed["Path"];
	}
	if(_full_path.find("//") != std::string::npos)
		_full_path.replace(_full_path.find("//"), 2, "/");
	if(_dir_path.find("//") != std::string::npos)
		_dir_path.replace(_dir_path.find("//"), 2, "/");
	if(_actual_root.find("//") != std::string::npos)
		_actual_root.replace(_actual_root.find("//"), 2, "/");
	if(_url_path.find("//") != std::string::npos)
		_url_path.replace(_url_path.find("//"), 2, "/");
	/*CGI funciona mas sem verificar input do server*/
	if (_full_path.find(".php") != std::string::npos)
	{
		CgiHandler	cgi_init;
		std::string	cgi_body;
		_body = cgi_init.cgiHandler(_full_path);
		return;
	}
	/*Checa se diretório não for acessivel */
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
		/*Checa se pasta não é acessivel*/
		if (dirCheck(_full_path) == 2)
			return ;
		/*Checa se o buffer vai ser gerado por arquivi o autoindex*/
		if (_configs.getDirList() == 0 || !dirCheck(_full_path))
			_body = buffer;
		if (_configs.getDirList() == 1 && dirCheck(_full_path))
			autoIndex();
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
		if (content <= 0)
		{
			printError("411", "Length Required");
			return ;
		}
		if((content)/1000 > _configs.getBodySizeLimit())
		{
			std::cerr << "Body size limit exceeded" << std::endl;
			printError("414", "URI Too Long");
			return ;
		}
//		if (_error_flag != 1)
			printHeader ("200", "OK", _req_parsed["Version"]);
		page.close();
	}
	else
		printError("404", "Not Found");
}

void	Response::methodDelete(std::map <std::string, std::string> _req_parsed)
{
	_full_path = _actual_root;
	_dir_path = _actual_root;
	/*limpa o path quando adicionar // */
	if(_full_path.find("//") != std::string::npos)
		_full_path.replace(_full_path.find("//"), 2, "/");
	/*Checa se o index existe caso exista acessa ele */
	if (access (((_full_path + + "/" + _configs.getIndex()).c_str()), F_OK) !=
		-1)
		_full_path = _full_path + "/" +  _configs.getIndex();
	FILE *file = fopen(_full_path.c_str(), "r");
	if(!file)
	{
		printError("404", "Not Found");
		return ;
	}
	else
	{
		std::remove (_full_path.c_str());
		printHeader ("200", "OK", _req_parsed["Version"]);
	}
}

/*Checa sé não é diretório para ligar autoindex*/
int	Response::dirCheck(std::string dir)
{
	struct stat buf;
	lstat(dir.c_str(), &buf);
	FILE *check_fp = fopen(dir.c_str(), "r");
	if (!check_fp)
	{
		std::cerr << "Forbidden" << std::endl;
		printError("403", "Forbidden");
		return (2);
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

void Response::_parseChunk(void)
{
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

void Response::_parseUrlEncodedParams(void)
{
	std::string	&params = this->_postBodyStr;

	// std::cout << "_postBodyStr:\n" << this->_postBodyStr << std::endl;

	size_t separatorPos = params.find('=');
	if (separatorPos == std::string::npos)
		throw std::runtime_error("error 400??? invalid application/x-www-form-urlencoded format");
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

		this->_vars[key] = value;
	}
	// std::map<std::string, std::string>::iterator it;
	// for(it = this->_vars.begin(); it != this->_vars.end(); it++)
	// 	std::cout << it->first << "=" << it->second << std::endl;

	// Estou deixando assim por enquanto só para que haja uma resposta e não dê algum erro
	printHeader ("200", "OK", this->_postHeaders["Version"]);
	return ;
}

void	Response::_parseTextPlain(void)
{
	// ver se é mesmo necessário chamar essa função nesse caso
	// _removeBreakLinesAndCR(textPlain);

	// Estou deixando assim por enquanto só para que haja uma resposta e não dê algum erro
	printHeader ("200", "OK", this->_postHeaders["Version"]);
	return ;
}

void	Response::_setBoundary(void)
{
	std::string &contentType = this->_postHeaders["Content-Type"];

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

void	Response::_setHeaders(void)
{
	size_t		npos;
	std::string	headers, headerKey, headerValue;

	// std::cout << "ANTES: size:" << this->_requestData.size() << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];
	if ((npos = _findSequence(this->_requestData, this->_boundary)) != 0)
		this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin() + npos + 2);
	else
	{
		std::cout << "headers error: " << std::endl << headers << std::endl;
		std::cout << "boundary: " << this->_boundary << std::endl;
		throw std::runtime_error("invalid multipart/formdata '_setHeaders'");
	}
	// std::cout << "DEPOIS:" << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];
	npos = _findSequence(this->_requestData, "\r\n\r\n");
	for (size_t i = 0; i < npos; i++)
	{
		headers += this->_requestData[0];
		this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin() + 1);
	}
	// std::cout << "DEPOIS DE NOVO:" << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];
	// std::cout << "headers:" << std::endl << headers << std::endl;
	_removeHeaderSpaces(headers);
	while (true)
	{
		if ((npos = headers.find(':')) != std::string::npos)
		{
			headerKey = headers.substr(0, npos);
			headers.erase(0, headerKey.size() + 1);
			if ((npos = headers.find("\r\n")) != std::string::npos)
			{
				headerValue = headers.substr(0, npos);
				headers.erase(0, headerValue.size());
			}
			else
				throw std::runtime_error("invalid multipart/formdata 'npos'");

			this->_boundaryHeaders[headerKey] = headerValue;
			if ((headers.size() >= 4) && (headers.substr(0, 4) == "\r\n\r\n"))
			{
				headers.erase(0, 4);
				return ;
				// std::cout << "MAP DAS HEADERS DO BOUNDARY:" << std::endl;
				// std::map<std::string, std::string>::iterator it;
				// for (it = this->_boundaryHeaders.begin(); it != this->_boundaryHeaders.end(); it++)
				// 	std::cout << it->first << "=" << it->second << std::endl;
			}
			else
				headers.erase(0, 2);
		}
		else
			throw std::runtime_error("invalid multipart/formdata 'npos'");
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

void	Response::_setPostBodyVector(void)
{
	size_t	boundaryStart = 2;
	int npos = _findSequence(this->_requestData, this->_boundary);
	if (npos > 0)
	{
		for (size_t i = 0; i < (npos - boundaryStart - this->_boundary.size()); i++)
		{
			this->_postBody.push_back(this->_requestData[0]);
			this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin() + 1);
		}
		// std::cout << "Result of requestData after boundary" << std::endl;
		// for (size_t i = 0; i < this->_requestData.size(); i++)
		// 	std::cout << this->_requestData[i];
	}
	else
		throw std::runtime_error("invalid request format '_setPostBodyVector'");
	return ;
}

void	Response::_handleImputFile(std::string &contentDisposition, t_serverConfig &serverConfig)
{
	std::ofstream	file;
	std::string		originalFileName;

	_getUploadDir(serverConfig);
	originalFileName = _originalFileName(contentDisposition);
	file.open(_generateFileName(originalFileName).c_str(), std::ios::out);
	if (!file.is_open())
		throw std::runtime_error("open file error '_handleImputFile'");
	_setPostBodyVector();
	for (size_t i = 0; i < this->_postBody.size(); i++)
		file << this->_postBody[i];
	// std::cout << "Result of requestData after boundary part" << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];
	// std::cout << "size: " << this->_boundary << std::endl;
	file.close();
	printHeader ("200", "OK", this->_postHeaders["Version"]);
	return ;
}

void	Response::_processBoundaryHeaders(t_serverConfig &serverConfig)
{
	if (this->_boundaryHeaders["Content-Disposition"].find("form-data;") != std::string::npos)
	{
		if (this->_boundaryHeaders["Content-Disposition"].find("filename=") != std::string::npos)
			_handleImputFile(this->_boundaryHeaders["Content-Disposition"], serverConfig);
		else
			throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	}
	else
		throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	return ;
}

void	Response::_handleBoundaryPart(t_serverConfig &serverConfig)
{
	_setHeaders();
	// std::map<std::string, std::string>::iterator it;
	// for(it = this->_boundaryHeaders.begin(); it != this->_boundaryHeaders.end(); it++)
	// 	std::cout << "key: " << it->first << " | value: " << it->second << std::endl;
	_processBoundaryHeaders(serverConfig);
}

void	Response::_parseMultipartFormData(t_serverConfig &serverConfig)
{
	_setBoundary();
	_handleBoundaryPart(serverConfig);
	return ;
}

void	Response::_methodPost(t_serverConfig &serverConfig)
{
	if (this->_postHeaders["Transfer-Encoding"] == "chunked")
		_parseChunk();
	if (this->_postHeaders["Content-Type"] == "application/x-www-form-urlencoded")
		_parseUrlEncodedParams();
	else if (this->_postHeaders["Content-Type"] == "text/plain")
		_parseTextPlain();
	else if (this->_postHeaders["Content-Type"].find("multipart/form-data") != std::string::npos)
		_parseMultipartFormData(serverConfig);
	else
		std::cerr << "ERROR: " << this->_postHeaders["Content-Type"] << std::endl;
	return ;
}

int	Response::_findSequence(std::vector<unsigned char> &vector, std::string const sequence)
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
		return (0);
	return (i);
}

std::string	Response::_setStringHeaders(void)
{
	std::string	postHeaders;

	if (this->_requestData.size() > 3)
	{
		int npos = _findSequence(this->_requestData, "\r\n\r\n");
		if (npos > 0)
		{
			for (int i = 0; i < npos; i++)
			{
				postHeaders += this->_requestData[0];
				this->_requestData.erase(this->_requestData.begin(), this->_requestData.begin() + 1);
			}
			// std::cout << "Header 1º: \n" << postHeaders << std::endl;
		}
		else
			throw std::runtime_error("invalid request format '_setPostHeaders'");
	}
	else
		throw std::runtime_error("invalid request format '_setPostHeaders'");
	return (postHeaders);
}

void	Response::_setPostBodyStr(void)
{
	for (size_t i = 0; i < this->_requestData.size(); i++)
		this->_postBodyStr += this->_requestData[i];
	return ;
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

	// std::cout << "_requestData:\n" << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];

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
	// std::cout << "POST HEADERS:\n" << std::endl;
	// std::map<std::string, std::string>::iterator	it;
	// for (it = _postHeaders.begin(); it != _postHeaders.end(); it++)
	// 	std::cout << it->first << "=" << it->second << std::endl;
	if (!(this->_postHeaders["Content-Type"].find("multipart/form-data") != std::string::npos))
		_setPostBodyStr();
	return ;
}

bool	Response::checkRequest()
{
	if (_req_parsed["Version"] != "HTTP/0.9" && _req_parsed["Version"] !=
		"HTTP/1.0" && _req_parsed["Version"] != "HTTP/1.1" &&
			_req_parsed["Version"] != "HTTP/2.0")
	{
		std::cerr << "Error 505 Http Version not supported" << std::endl;
		printError("505", "HTTP Version Not Supported");
		return (1);
	}
	if (!_configs.getHttpMethods().empty() && _configs.getHttpMethods().find(_req_parsed["Method"]) == _configs.
		getHttpMethods().end())
	{
		std::cerr << "Error 405 invalid method" << std::endl;
		printError("405", "Method Not Allowed");
		return (1);
	}
	return (0);
}
void	Response::init()
{
	/*Iniciando o server com os dados do path selecionado*/
	std::string url;
	std::string _clean_address;

	url = _req_parsed["Path"].substr(0,_req_parsed["Path"].find('/', 1));
	if (_req_parsed["Path"] != url)
		_clean_address =  _req_parsed["Path"].substr(_req_parsed["Path"].find
			('/', 1),_req_parsed["Path"].size());
	if (_serverConfig.routes.find(url) != _serverConfig.routes.end())
	{
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[url]);
		_url_path = _req_parsed["Path"];
	}
	else
	{
		_configs = ServerConfig(_serverConfig);
		_url_path = _req_parsed["Path"];
	}
	if (_serverConfig.routes.find(_url_path) != _serverConfig.routes.end())
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[_url_path]);
	_actual_root = _configs.getRoot() + _clean_address;
	// std::cout <<"Root:"<< _configs.getRoot()<< std::endl;
	// std::cout <<"Autoindex:"<< _configs.getDirList() << std::endl;
	// std::cout <<"Indexx:"<< _configs.getIndex()<< std::endl;
	// std::cout <<"LimitSize:"<< _configs.getBodySizeLimit()<< std::endl;
	// std::cout <<"Redirect:"<< _configs.getRedirect() << std::endl;
	// std::cout <<"UploadPath:"<< _configs.getUploadPath() << std::endl;
	// std::cout <<"Req_parsed_path"<< _req_parsed["Path"]<< std::endl;
	// std::cout <<"Url path :"<<  _url_path<< std::endl;
	/*checa se o método solicitado está incluso no location*/
	if (checkRequest())
		return ;
	if (_req_parsed["Method"] == "GET")
		methodGet(_req_parsed);
	if (_req_parsed["Method"] == "DELETE")
		methodDelete(_req_parsed);
	// std::cout << "ANTES DE MODIFICAR - SIZE: " << this->_requestData.size() << std::endl;
	// for (size_t i = 0; i < this->_requestData.size(); i++)
	// 	std::cout << this->_requestData[i];
	_parseRequestData();
	// std::map<std::string, std::string>::iterator it;
	// for (it = this->_postHeaders.begin(); it != this->_postHeaders.end(); it++)
	// 	std::cout << it->first << "=" << it->second << std::endl;
	if (this->_postHeaders["Method"] == "POST")
		_methodPost(_serverConfig);
	// throw std::runtime_error("EXIT");
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

