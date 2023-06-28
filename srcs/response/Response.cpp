#include "Response.hpp"

Response::Response(std::map <std::string, std::string>& _res_param_, std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_, std::string& _actual_root_,
std::vector<unsigned char> &requestData)
:_res_param(_res_param_), _req_parsed(_req_parsed_), _serverConfig
(_serverConfig_), _actual_root(_actual_root_), _requestData(requestData)
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
//			std::cout << " Root: "<< _res_param["Root"] << " Path : "<<_req_parsed["Path"] << " dir_path:" <<  _dir_path;
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

void	Response::methodGet(std::map <std::string, std::string> _req_parsed,
	std::map <std::string, std::string> _res_param)
{
	if (_req_parsed["Path"] == "/")
	{
		_full_path = _res_param["Root"] + _req_parsed["Path"] + _res_param
			["Index"];
		_dir_path = _res_param["Root"] + _req_parsed["Path"];
	}
	// else if (_index_flag == 1)
	// {
	// 	_full_path = _res_param["Root"] + _res_param["Index"];
	// 	_dir_path = _res_param["Root"];
	// }
	else if (_path_flag == 1)
	{
		std::string		temp = _req_parsed["Path"];
		std::string::size_type i = temp.find(_path_location);
		if (i != std::string::npos)
		{
			temp.erase(i, _path_location.length());
		}
		_full_path = _res_param["Root"] + temp;
		_dir_path = _res_param["Root"] + temp;
	}
	else
	{
		_full_path = _res_param["Root"] + _req_parsed["Path"];
		_dir_path = _res_param["Root"]+ _req_parsed["Path"];
	}
	/*CGI funciona mas sem verificar input do server*/
	if (_full_path.find(".php") != std::string::npos)
	{
		CgiHandler	cgi_init;
		std::string	cgi_body;
		std::cout << "cgi entro \n\n\n";
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
		if (_res_param["AutoIndex"] == "off")
			_body = buffer;
		if (_res_param["AutoIndex"] == "on")
			autoIndex(_res_param);
		/*Converte o Content-length de size_t pra string e adiciona no map*/
		std::stringstream sizet_len;
		sizet_len <<  buffer.size();
		std::string string_len = sizet_len.str();
		if (_res_param["AutoIndex"] == "off")
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		else if (_res_param["AutoIndex"] == "on")
		{
			std::stringstream sizet_len;
			sizet_len <<  _body.size();
			std::string string_len = sizet_len.str();
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		}
		/*Verifica o Content-type do arquivo*/
		_res_map["Content-type"] = getType();
		if(_res_map["Content-Length"] > _res_param["bodySizeLimit"])
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

void	Response::locationCheck()
{
	std::string temp;
	std::map<std::string, t_route>::iterator itr;
	for(itr=_serverConfig.routes.begin();itr!=_serverConfig.routes.end();itr++)
	{
//		std::cout << "Actual: " << _actual_root << "\n req_PATH: " << _req_parsed["Path"] << "\n itr->sec: " <<itr->second.root << std::endl;
 		_res_param["AutoIndex" ] ="on";
		if ("/" == _req_parsed["Path"])
			_actual_root = _serverConfig.root;
		if (itr->first == _req_parsed["Path"])
		{	/*checa se o método solicitado está incluso no location*/
			if (!(itr->second.httpMethods.find(_req_parsed["Method"]) !=
				itr->second.httpMethods.end()))
				std::cout << "ERROR INVALID METHOD" << std::endl;
			if (itr->second.root != "")
			 	_actual_root = itr->second.root;
			else
			 	_actual_root = _serverConfig.root;
			if (itr->second.dirList)
			{
					struct stat buf;
					std::string dir = _actual_root + "/" + itr->second.index;
					lstat(dir.c_str(), &buf);
					std::cout << dir;
					FILE *check_fp = fopen(dir.c_str(), "r");
					if (!check_fp)
						std::cout << "Error 404";
					else
					{
						fclose(check_fp);
						if (S_ISDIR(buf.st_mode))
							_res_param["AutoIndex"] ="on";
						else
						{
							_res_param["AutoIndex"] ="off";
							_req_parsed["Path"] = "/" + itr->second.index;
						}
					}
			}
			else
				_res_param["AutoIndex" ] ="off";
		}
		if ((_actual_root != _req_parsed["Path"] && itr->second.root !=
			_req_parsed["Path"] && itr->first == _req_parsed["Path"]) ||
				_req_parsed["Path"].find(itr->first) != std::string::npos)
		{
			_path_flag = 1;
			_path_location = itr->first;
			std::string		temp = _req_parsed["Path"];
			std::string::size_type i = temp.find(_path_location);
			if (i != std::string::npos)
				temp.erase(i, _path_location.length());
			struct stat buf;
			std::string dir = (_actual_root + temp);
			if(dir.find("//") != std::string::npos)
				dir.replace(dir.find("//"), 2, "/");
			lstat(dir.c_str(), &buf);
			FILE *check_fp = fopen(dir.c_str(), "r");
			if (!check_fp)
				std::cout << "Error 404";
			else
			{
				fclose(check_fp);
				if (S_ISDIR(buf.st_mode))
					_res_param["AutoIndex"] ="on";
				else
					_res_param["AutoIndex"] ="off";
			}
			break;
		}
		/*Checa sé não é diretório para ligar autoindex*/
		std::string		temp = _req_parsed["Path"];
		std::string::size_type i = temp.find(_path_location);
		if (i != std::string::npos)
			temp.erase(i, _path_location.length());
		struct stat buf;
		std::string dir = (_actual_root + temp);
		lstat(dir.c_str(), &buf);
		FILE *check_fp = fopen(dir.c_str(), "r");
		if (!check_fp)
			std::cout << "Error 404";
		else
		{
			fclose(check_fp);
			if (S_ISDIR(buf.st_mode))
				_res_param["AutoIndex"] ="on";
			else
				_res_param["AutoIndex"] ="off";
		}
		if (itr->first == _req_parsed["Path"])
			break;
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

	if (this->_mapImput["Path"] == "/")
		throw std::runtime_error("error 405 '_getDir'");
	else
	{
		if (((it = serverConfig.routes.find(this->_mapImput["Path"])) != serverConfig.routes.end())
		|| ((it = serverConfig.routes.find(_handleLastSlash(this->_mapImput["Path"]))) != serverConfig.routes.end()))
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
	// std::cout << multipart;
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
	
	// std::map<std::string, std::string>::iterator it;
	// for(it = map_input.begin(); it != map_input.end(); it++)
	// 	std::cout << "key: " << it->first << " | value: " << it->second << std::endl;
	return ;
}

void	Response::init(int _flag)
{
	if (_flag == 1)
	{
	 	_actual_root = _serverConfig.root;
	 	_flag = 1;
	}
	else
	{
		_path_flag = 0;
		_index_flag = 0;
		_res_param.insert(std::pair<std::string,std::string>("Index",
			_serverConfig.
			index));
		_res_param.insert(std::pair<std::string,std::string>("bodySizeLimit",
			intToString(_serverConfig.bodySizeLimit)) );
		_res_param.insert(std::pair<std::string,std::string>("AutoIndex",""));
		locationCheck();
		_res_param.insert(std::pair<std::string,std::string>("Root",
			_actual_root));
		if (_req_parsed["Method"] == "GET")
			methodGet(_req_parsed, _res_param);
		if (_req_parsed["Method"] == "DELETE")
			methodDelete(_req_parsed, _res_param);
	}
	if (_req_parsed["Method"] == "POST")
	{
		this->_mapImput = _req_parsed;
		_methodPost(_req_parsed, _serverConfig);
		std::cout << "Depois: " << std::endl;
		for (size_t i = 0; i < this->_requestData.size(); i++)
			std::cout << this->_requestData[i];
	}

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
