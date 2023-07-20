#include "Response.hpp"

Response::Response(std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_,
std::string&_url_path_, std::string &strBody, std::vector<unsigned char> &vectorBody, std::string &_actual_root_) :
_req_parsed(_req_parsed_), _serverConfig (_serverConfig_),
_url_path(_url_path_), _strBody(strBody), _vectorBody(vectorBody), _actual_root(_actual_root_)
{
}

/*Salva a resposta e imprimi no terminal*/
void	Response::printHeader(std::string status_code, std::string message,
	std::string http_version)
{
	std::time_t result = std::time(NULL);
	result = result + (1 * 60 * 60);
	std::string timeString = std::asctime(std::localtime(&result));
	_response.append(http_version + " " + status_code + " " + message +  "\r\n");
	for (std::map<std::string, std::string>::iterator i = _res_map.begin();
		i != _res_map.end(); i++)
		_response.append((*i).first + ": " + (*i).second + "\r\n");
	if (_req_parsed["Query"] != "")
	{
		_response.append("Set-Cookie: " + _req_parsed["Query"] +"\r\n");
		_response.append("Set-Cookie: " + _req_parsed["Query"] +"\r\n");
	}
	_response.append("Set-Cookie: HttpOnly=true\r\n");
	_response.append("Set-Cookie: Secure=true\r\n");
	_response.append("Set-Cookie: Domain=" + _req_parsed["Host"] + "\r\n");
	_response.append("Set-Cookie: Path=" + _req_parsed["Path"] + "\r\n");
	_response.append("Set-Cookie: Expires=" + timeString + "\r\n");
	_response.append(_body);
	_response.append("\r\n");
	//	_response.append("\r\n"); // deixar apenas essa linha sem uso de cookie
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
			std::cerr << "Page Not Found"<< std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_404, _configs.
				getErrorPage(ERROR_404));
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
		_response.append(_body);
		_response.append("\r\n");
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
			std::cerr << "ContentL Length Required" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_411, _configs.
				getErrorPage(ERROR_411));
			return ;
		}
		if((content)/1000 > _configs.getBodySizeLimit())
		{
			std::cerr << "Body size limit exceeded" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_414, _configs.
				getErrorPage(ERROR_414));
			return ;
		}
		printHeader ("200", "OK", _req_parsed["Version"]);
		page.close();
	}
	else
	{
		std::cerr << "Page Not Found"<< std::endl;
		_response = ErrorResponse::getErrorResponse(ERROR_404, _configs.
			getErrorPage(ERROR_404));
	}
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
		std::cerr << "Page Not Found"<< std::endl;
		_response = ErrorResponse::getErrorResponse(ERROR_404, _configs.
			getErrorPage(ERROR_404));
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
		_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
			getErrorPage(ERROR_403));
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
	std::string	&params = this->_strBody;

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
	// Estou deixando assim por enquanto só para que haja uma resposta e não dê algum erro
	printHeader ("200", "OK", this->_req_parsed["Version"]);
	return ;
}

void	Response::_parseTextPlain(void)
{
	// ver se é mesmo necessário chamar essa função nesse caso
	// _removeBreakLinesAndCR(textPlain);

	// Estou deixando assim por enquanto só para que haja uma resposta e não dê algum erro
	printHeader ("200", "OK", this->_req_parsed["Version"]);
	return ;
}

void	Response::_setBoundary(void)
{
	std::string &contentType = this->_req_parsed["Content-Type"];

	size_t boundaryPos;
	if ((boundaryPos = contentType.find("---")) != std::string::npos)
		this->_boundary = contentType.substr(boundaryPos, contentType.size() - boundaryPos);
	else
		throw std::runtime_error("boundary not found");
	return ;
}

void	Response::_removeHeaderSpaces(std::string &header)
{
	size_t		endHeaderPos, i = 0;
	std::string	tempHeader;

	if ((endHeaderPos = header.find("\r\n\r\n")) != std::string::npos)
	{
		while (i < endHeaderPos)
		{
			if (header[i] != ' ')
				tempHeader += header[i];
			i++;
		}
		while (i < header.size())
		{
			tempHeader += header[i];
			i++;
		}
		header = tempHeader;
	}
	else
		throw std::runtime_error("invalid multipart/formdata 'endHeaderPos'");

	return ;
}

size_t	Response::_findSequence(std::vector<unsigned char> &vector, std::string const sequence)
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

void	Response::_setHeaders(void)
{
	size_t		npos;
	size_t		endHeader = 4;
	std::string	headers, headerKey, headerValue;

	if ((npos = _findSequence(this->_vectorBody, this->_boundary)) != std::string::npos)
		this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + npos + this->_boundary.size() + 2);
	else
	{
		std::cerr << "headers error: " << std::endl << headers << std::endl;
		std::cerr << "boundary: " << this->_boundary << std::endl;
		throw std::runtime_error("invalid multipart/formdata '_setHeaders'");
	}
	npos = _findSequence(this->_vectorBody, "\r\n\r\n");
	for (size_t i = 0; i < (npos + endHeader); i++)
	{
		headers += this->_vectorBody[0];
		this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
	}
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

// std::string	Response::_handleLastSlash(std::string &Route)
// {
// 	std::string	alternativeRoute;

// 	if (Route[Route.size() - 1] != '/')
// 		alternativeRoute = Route + '/';
// 	else
// 		alternativeRoute = Route.substr(0, Route.size() - 1);
// 	std::cout << "alternative: " << alternativeRoute << std::endl;
// 	return (alternativeRoute);
// }

void	Response::_setBoundaryBody(void)
{
	size_t npos;
	size_t	boundaryStart = 2;

	if ((npos = _findSequence(this->_vectorBody, this->_boundary)) != std::string::npos)
	{
		for (size_t i = 0; i < (npos - boundaryStart); i++)
		{
			this->_vectorBoundaryBody.push_back(this->_vectorBody[0]);
			this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
		}
	}
	else
		throw std::runtime_error("invalid request format '_setBoundaryBody'");
	return ;
}

void	Response::_handleImputFile(std::string &contentDisposition)
{
	size_t			pos;
	std::ofstream	file;
	std::string		originalFileName;
	std::string		fileName = _configs.getUploadPath();

	pos = _configs.getUploadPath().size() - 1;
	if (_configs.getUploadPath().find('/', pos) == std::string::npos)
		fileName += '/';
	originalFileName = _generateFileName(originalFileName);
	originalFileName += _originalFileName(contentDisposition);
	fileName += originalFileName;

	file.open(fileName.c_str(), std::ios::out);
	if (!file.is_open())
		throw std::runtime_error("open file error '_handleImputFile'");
	_setBoundaryBody();
	for (size_t i = 0; i < this->_vectorBoundaryBody.size(); i++)
		file << this->_vectorBoundaryBody[i];
	file.close();
	printHeader ("200", "OK", this->_req_parsed["Version"]);
	return ;
}

void	Response::_processBoundaryHeaders(void)
{
	if (this->_boundaryHeaders["Content-Disposition"].find("form-data;") != std::string::npos)
	{
		if (this->_boundaryHeaders["Content-Disposition"].find("filename=") != std::string::npos)
			_handleImputFile(this->_boundaryHeaders["Content-Disposition"]);
		else
			throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	}
	else
		throw std::runtime_error("invalid multipart/formdata '_processBoundaryHeaders'");
	return ;
}

void	Response::_handleBoundaryPart(void)
{
	_setHeaders();
	_processBoundaryHeaders();
}

void	Response::_parseMultipartFormData(void)
{
	_setBoundary();
	_handleBoundaryPart();
	return ;
}

void	Response::_methodPost(void)
{
	if (this->_req_parsed["Transfer-Encoding"] == "chunked")
		_parseChunk();
	if (this->_req_parsed["Content-Type"] == "application/x-www-form-urlencoded")
		_parseUrlEncodedParams();
	else if (this->_req_parsed["Content-Type"] == "text/plain")
		_parseTextPlain();
	else if (this->_req_parsed["Content-Type"].find("multipart/form-data") != std::string::npos)
		_parseMultipartFormData();
	else
		std::cerr << "ERROR: " << this->_req_parsed["Content-Type"] << std::endl;
	return ;
}

bool	Response::checkRequest()
{
	if ( _req_parsed["Version"] != "HTTP/0.9" && _req_parsed["Version"] !=
		"HTTP/1.0" && _req_parsed["Version"] != "HTTP/1.1" &&
			_req_parsed["Version"] != "HTTP/2.0")
	{
		if (_req_parsed["Version"].substr(0, 4) == "HTTP/")
		{
			std::cerr << "Error 505 Http Version not supported" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_505, _configs.
			getErrorPage(ERROR_505));
		}
		else
		{
			std::cerr << "Error 400 Bad Request" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
		}
		return (true);
	}
	if (!_configs.getHttpMethods().empty() && _configs.getHttpMethods().find
		(_req_parsed["Method"]) == _configs.
		getHttpMethods().end())
	{
		std::cerr << "Error 405 invalid method" << std::endl;
		_response = ErrorResponse::getErrorResponse(ERROR_405, _configs.
			getErrorPage(ERROR_405));
		return (true);
	}
	return (false);
}

void	Response::getCookie()
{
	std::string name;
	if ( _req_parsed["Cookie"].find("name=") != std::string::npos)
	{
		size_t start =  _req_parsed["Cookie"].find("name=");
		size_t end =  _req_parsed["Cookie"].find(";", start);
		name = _req_parsed["Cookie"].substr(start + 5, end - start -5);
	}
		else
			name = "Enter a query name='your name';
	std::ofstream WriteFile("index/file.txt");
	WriteFile << name;
}

void	Response::init()
{
	/*Iniciando o server com os dados do path selecionado*/
	if (_req_parsed["Cookie"] != "")
		getCookie();
	std::string url;
	std::string _clean_address;
	url = _req_parsed["Path"].substr(0,_req_parsed["Path"].find('/', 1));
	if (_req_parsed["Path"] != url)
		_clean_address =  _req_parsed["Path"].substr(_req_parsed["Path"].find
			('/', 1));
	if (_serverConfig.routes.find(url) != _serverConfig.routes.end())
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[url]);
	else if (access (((_actual_root + "/" + _configs.getIndex()).c_str()),
		F_OK) != -1 && _serverConfig.routes.find(_url_path) != _serverConfig.
			routes.end() && url != "/")
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[_url_path]);
	else
		_configs = ServerConfig(_serverConfig);
	_url_path = _req_parsed["Path"];
	/*Checa se o arquivo esta no url ou se é um GET proveniente de pagina html*/
	if (_serverConfig.routes.find(url) != _serverConfig.routes.end())
		_actual_root = _configs.getRoot() + _clean_address;
	else
		_actual_root = _configs.getRoot() + _req_parsed["Path"];
	// std::cout <<"Actual Root:"<< _actual_root<< std::endl;
	// std::cout << "url: "<<  url << std::endl;
	// std::cout <<"Root:"<< _configs.getRoot()<< std::endl;
	// std::cout <<"Autoindex:"<< _configs.getDirList() << std::endl;
	// std::cout <<"Indexx:"<< _configs.getIndex()<< std::endl;
	// std::cout <<"LimitSize:"<< _configs.getBodySizeLimit()<< std::endl;
	// std::cout <<"Redirect:"<< _configs.getRedirect() << std::endl;
	// std::cout <<"UploadPath:"<< _configs.getUploadPath() << std::endl;
	// std::cout <<"Req_parsed_path"<< _req_parsed["Path"]<< std::endl;
	// std::cout <<"Url path :"<<  _url_path<< std::endl;
	/*checa se o método solicitado está incluso no location*/

	// std::cout << "headers RESPONSE:" << std::endl;
	// std::map<std::string, std::string>::iterator it;
	// for (it = this->_req_parsed.begin(); it != this->_req_parsed.end(); it++)
	// 	std::cout << it->first << "=" << it->second << std::endl;
	// std::cout << std::endl;
	// std::cout << "_vectorBody RESPONSE:" << std::endl;
	// for (size_t i = 0; i < _vectorBody.size(); i++)
	// 	std::cout << _vectorBody[i];
	// std::cout << std::endl;
	// std::cout << "_strBody RESPONSE:" << std::endl;
	// std::cout << _strBody << std::endl;
	if (checkRequest())
		return ;
	if (_req_parsed["Method"] == "GET")
		methodGet(_req_parsed);
	if (_req_parsed["Method"] == "DELETE")
		methodDelete(_req_parsed);
	if (this->_req_parsed["Method"] == "POST")
		_methodPost();
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

