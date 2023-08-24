#include "Response.hpp"

Response::Response(std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_,
std::string&_url_path_, std::string &strBody, std::vector<unsigned char> &vectorBody, std::string &_actual_root_) :
_req_parsed(_req_parsed_), _serverConfig (_serverConfig_),
_url_path(_url_path_), _strBody(strBody), _vectorBody(vectorBody), _actual_root(_actual_root_)
{
	_phpSuffix = "";
	_pythonSuffix = "";
	_suffix = "";
	return ;
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
		i != _res_map.end(); i++){
		_response.append((*i).first + ": " + (*i).second + "\r\n");
		std::cout << ((*i).first + ": " + (*i).second + "\r\n");}
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
		_checkCgiRequest();
		if (_suffix == "")
		{
			std::cerr << "Forbidden" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
			return ;
		}
		CgiHandler	cgi_init("",  _full_path, _configs.getCgi(_suffix), this->_req_parsed, _response, _configs);
		std::string	cgiResult;
		_body = cgi_init.cgiHandler();
		_response.append(_body);
		_response.append("\r\n");
		return ;
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
			std::cerr << "Content Length Required" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_411, _configs.
				getErrorPage(ERROR_411));
			return ;
		}
		if((content)/1000 > _configs.getBodySizeLimit())
		{
			std::cerr << "Body size limit exceeded" << std::endl;
			_response = ErrorResponse::getErrorResponse(ERROR_413, _configs.
				getErrorPage(ERROR_413));
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
		size_t	npos = params.find('%', pos);
		if (npos == std::string::npos)
            break ;
		else
		{
			if ((params.size() - npos) > 2)
			{
				std::string hexStr;
				hexStr = params.substr(npos + 1, 2);

				int	value;
				std::istringstream	iss(hexStr);
				iss >> std::hex >> value;
				if (iss.fail())
				{
					_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
					getErrorPage(ERROR_400));
					throw std::runtime_error("400 Bad Request (failed to convert hex to ASCII)");
				}
				std::string asciiChar;
				asciiChar.push_back(static_cast<char>(value));
				params.replace(npos, 3, asciiChar);
			}
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
				getErrorPage(ERROR_400));
				throw std::runtime_error("400 Bad Request (invalid parameters format)");
			}
		}
	}
	return ;
}

void Response::_replacePlusWithSpace(std::string &params)
{
	int	pos = 0;
	while (true)
	{
		size_t	npos = params.find('+', pos);
		if (npos == std::string::npos)
            break ;
		else
			params.replace(npos, 1, " ");
	}
	return ;
}

void Response::_parseUrlEncodedParams(void)
{
	size_t separatorPos = this->_strBody.find('=');
	if (separatorPos == std::string::npos)
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (this->_strBody application/x-www-form-urlencoded format)");
	}
	_removeBreakLinesAndCR (this->_strBody);
	_replaceHexPercentWithAscii(this->_strBody);
	_replacePlusWithSpace(this->_strBody);
	_sendDataToHandlerCGI();
	printHeader ("200", "OK", _req_parsed["Version"]);
	this->_req_parsed.clear();
	return ;
}

void	Response::_sendDataToHandlerCGI(void)
{
	std::ofstream	file;
	std::string		fileName;

	fileName = _generateFileName("temp_", "_cgi");
	file.open(fileName.c_str(), std::ofstream::in | std::ofstream::trunc);
	if (!file.is_open())
	{
		_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
		getErrorPage(ERROR_500));
		throw std::runtime_error("500 Internal Server Error (open/_handleBoundaryFiles)");
	}
	file.write(this->_strBody.c_str(), this->_strBody.size());
	file.close();
	// aqui eu tiro a barra da url para que apenas o caminho relativo seja enviado ao execve
	if ((this->_url_path.size() > 1) && (this->_url_path[0] == '/'))
		this->_url_path.erase(0, 1);
	CgiHandler	cgi(fileName, this->_url_path, _configs.getCgi(_suffix), this->_req_parsed, _response, _configs);
	std::string	cgiResult;
	cgiResult = cgi.cgiHandler();
	remove(fileName.c_str());

	// _response = cgiResult;
	std::cout << "Resultado do CGI:\n" << cgiResult << std::endl;
	return ;
}

void	Response::_parseTextPlain(void)
{
	_sendDataToHandlerCGI();
	printHeader ("200", "OK", _req_parsed["Version"]);
	this->_req_parsed.clear();
	return ;
}

void	Response::_setBoundary(void)
{
	std::string &contentType = this->_req_parsed["Content-Type"];

	size_t boundaryPos;
	if ((boundaryPos = contentType.find("---")) != std::string::npos)
		this->_boundary += contentType.substr(boundaryPos, contentType.size() - boundaryPos);
	else
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (boundary not found)");
	}
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
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (invalid multipart/formdata/endHeaderPos)");
	}

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

std::string	Response::_originalFileName(std::string &contentDisposition)
{
	std::string	originalFileName = "_";
	std::string	fieldFileName = "filename=\"";

	size_t	npos;
	if ((npos = contentDisposition.find(fieldFileName)) != std::string::npos)
	{
		int	i = npos + fieldFileName.size();
		while ((contentDisposition[i]) && (contentDisposition[i] != '\"'))
			originalFileName += contentDisposition[i++];
	}
	else
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_originalFileName()/_fileExtension)");
	}
	return (originalFileName);
}

std::string	Response::_generateFileName(std::string const &type, std::string const &originalFileName)
{
	static int	sequenceNumber = 0;
	std::time_t	currentTime;
	std::stringstream	fileName;

	currentTime = std::time(0);
	fileName << type << currentTime << sequenceNumber << originalFileName;
	sequenceNumber++;
	return (fileName.str());
}

void	Response::_handleBoundaryFiles(void)
{
	std::string	carriageReturn = "\r\n\r\n";
	std::string	headers;
	std::string	boundaryEnd = this->_boundary + "--";
	size_t		boundaryStart = 2;
	size_t		npos;

	this->_strBody.clear();
	while (this->_strBody.find(boundaryEnd) == std::string::npos)
	{
		if (this->_vectorBody.size() == 0)
			return ;
		if ((npos = _findSequence(this->_vectorBody, this->_boundary)) == std::string::npos)
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleBoundaryFiles)");
		}
		if ((npos = _findSequence(this->_vectorBody, carriageReturn)) == std::string::npos)
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (invalid multipart/formdata (carriageReturn/_handleBoundaryFiles)");
		}
		for (size_t i = 0; i < (npos + carriageReturn.size()); i++)
		{
			headers += this->_vectorBody[0];
			this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
		}
		// É um arquivo
		if (headers.find("; filename=") != std::string::npos)
		{
			std::ofstream	file;
			std::string		fileName;

			this->_strBody += headers;
			fileName = _generateFileName("temp_", _originalFileName(headers));
			headers.clear();
			if ((npos = _findSequence(this->_vectorBody, this->_boundary)) == std::string::npos)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
				getErrorPage(ERROR_400));
				throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleBoundaryFiles)");
			}
			file.open(fileName.c_str(), std::ofstream::in | std::ofstream::trunc);
			if (!file.is_open())
			{
				_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
				getErrorPage(ERROR_500));
				throw std::runtime_error("500 Internal Server Error (open/_handleBoundaryFiles)");
			}
			for (size_t i = 0; i < (npos - boundaryStart); i++)
			{
				file << this->_vectorBody[0];
				this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
			}
			this->_strBody += "./" + fileName + '\n';
		}
		// Não é um arquivo
		else
		{
			this->_strBody += headers;
			headers.clear();
			if ((npos = _findSequence(this->_vectorBody, this->_boundary)) == std::string::npos)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
				getErrorPage(ERROR_400));
				throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleBoundaryFiles)");
			}
			for (size_t i = 0; i < (npos - boundaryStart); i++)
			{
				this->_strBody += this->_vectorBody[0];
				this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
			}
		}
		if ((npos = _findSequence(this->_vectorBody, this->_boundary + "\r\n")) == std::string::npos)
		{
			for (size_t i = 0; i < this->_vectorBody.size(); i++)
				this->_strBody += this->_vectorBody[i];
			this->_vectorBody.clear();
		}
	}

	_sendDataToHandlerCGI();
	printHeader ("200", "OK", _req_parsed["Version"]);
	this->_req_parsed.clear();
	return ;
}

void	Response::_parseMultipartFormData(void)
{
	_setBoundary();
	_handleBoundaryFiles();
	return ;
}

void	Response::_handleMultipart(std::string &fileName)
{
	std::ofstream				file;
	std::string					originalFileName;
	std::string					carriageReturn = "\r\n\r\n";
	std::string					headers;
	std::string					boundaryEnd = this->_boundary + "--";
	size_t						boundaryStart = 2;
	size_t						npos;
	size_t						count;
	std::vector<unsigned char>	check;

	if (this->_vectorBody.size() == 0)
		return ;
	if ((npos = _findSequence(this->_vectorBody, this->_boundary)) == std::string::npos)
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleMultipart/boundary not found)");
	}
	if ((npos = _findSequence(this->_vectorBody, carriageReturn)) == std::string::npos)
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (invalid multipart/formdata (carriageReturn/_handleMultipart)");
	}
	for (size_t i = 0; i < (npos + carriageReturn.size()); i++)
	{
		headers += this->_vectorBody[0];
		this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
	}
	// É um arquivo
	if (headers.find("; filename=") != std::string::npos)
	{
		if ((npos = _findSequence(this->_vectorBody, this->_boundary)) == std::string::npos)
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleMultipart/boundary not found 2)");
		}

		for (count = (npos - boundaryStart); count < this->_vectorBody.size(); count++)
			check.push_back(this->_vectorBody[count]);
		if ((_findSequence(check, this->_boundary + "--")) != 2)
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleMultipart/final boundary)");
		}

		fileName += _generateFileName("file_", _originalFileName(headers));
		file.open(fileName.c_str(), std::ofstream::out | std::ofstream::trunc);
		for (size_t i = 0; i < (npos - boundaryStart); i++)
		{
			file << this->_vectorBody[0];
			this->_vectorBody.erase(this->_vectorBody.begin(), this->_vectorBody.begin() + 1);
		}
	}
	// is not a file
	else
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (invalid multipart/formdata (_boundary/_handleMultipart/is not a file)");
	}
	printHeader ("200", "OK", _req_parsed["Version"]);
	this->_req_parsed.clear();
	return ;
}

void	Response::_isNotCGI(void)
{
	int				err;
	std::string		fileName;
	std::string		originalFileName;
	std::ofstream	file;
	std::string		uploadPath = "./uploads/others/";
	struct stat		st;

	if (_configs.getUploadPath().size() > 0)
	{
		if (stat(_configs.getUploadPath().c_str(), &st) != 0)
		{
			if (errno == ENOENT)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (isNotCGI/ENOENT)");
			}
			else if (errno == EACCES)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (isNotCGI/EACCES)");
			}
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
				getErrorPage(ERROR_500));
				throw std::runtime_error("500 Internal Server Error (uploads) 'isNotCGI'");
			}
		}
		else
			uploadPath = _configs.getUploadPath();
	}
	else
	{
		if (stat("uploads", &st) != 0)
		{
			if (errno == ENOENT)
			{
				if ((err = mkdir("uploads", 0777)) != 0)
				{
					_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
					getErrorPage(ERROR_500));
					throw std::runtime_error("500 Internal Server Error (uploads) 'isNotCGI'");
				}
			}
			else if (errno == EACCES)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (uploads) 'isNotCGI'");
			}
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
				getErrorPage(ERROR_500));
				throw std::runtime_error("500 Internal Server Error (uploads) 'isNotCGI'");
			}
		}
		if (stat("uploads/others", &st) != 0)
		{
			if (errno == ENOENT)
			{
				if ((err = mkdir("./uploads/others", 0777)) != 0)
				{
					_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
					getErrorPage(ERROR_500));
					throw std::runtime_error("500 Internal Server Error (others) 'isNotCGI'");
				}
			}
			else if (errno == EACCES)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (others) 'isNotCGI'");
			}
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
				getErrorPage(ERROR_500));
				throw std::runtime_error("500 Internal Server Error (others) 'isNotCGI'");
			}
		}
	}

	if (_configs.getUploadPath() == "")
		fileName =  uploadPath;
	else
	{
		fileName += _configs.getUploadPath();
		if (_configs.getUploadPath()[_configs.getUploadPath().size() - 1] != '/')
			fileName += '/';
	}

	if (this->_req_parsed["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		_setBoundary();
		_handleMultipart(fileName);
	}
	else
	{
		originalFileName = "_" + _url_path.substr(_url_path.find_last_of('/') + 1, _url_path.size());
		fileName += _generateFileName("file_", originalFileName);
		file.open(fileName.c_str(), std::ofstream::out | std::ofstream::trunc);
		for (size_t i = 0; i < this->_vectorBody.size(); i++)
			file << this->_vectorBody[i];
	}
	return ;
}

void	Response::_checkCgiRequest(void)
{
	try
	{
		if (_configs.getCgi(".php") != "")
			_phpSuffix = ".php";
	}
	catch(const std::out_of_range &e) {}

	try
	{
		if (_configs.getCgi(".py") != "")
			_pythonSuffix = ".py";
	}
	catch(const std::out_of_range &e) {}

	if ((_phpSuffix == "") && (_pythonSuffix == ""))
	{
		_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
		getErrorPage(ERROR_500));
		throw std::runtime_error("500 Internal Server Error (_checkCgiRequest)");
	}
	else if (((_phpSuffix == "")
	|| (_url_path.compare((_url_path.size() - _phpSuffix.size()), _phpSuffix.size(), _phpSuffix) != 0))
	&& ((_pythonSuffix == "")
	|| (_url_path.compare((_url_path.size() - _pythonSuffix.size()), _pythonSuffix.size(), _pythonSuffix) != 0)))
	{
		_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
		getErrorPage(ERROR_400));
		throw std::runtime_error("400 Bad Request (_methodPost / suffix getCgi and url are different)");
	}

	if (_url_path.compare((_url_path.size() - _phpSuffix.size()), _phpSuffix.size(), _phpSuffix) == 0)
		_suffix = _phpSuffix;
	else
		_suffix = _pythonSuffix;
	return ;
}

void	Response::_checkUploadPath(void)
{
	struct stat		st;

	if (_configs.getUploadPath().size() > 0)
	{
		if (stat(_configs.getUploadPath().c_str(), &st) != 0)
		{
			if (errno == ENOENT)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (_checkUploadPath/ENOENT)");
			}
			else if (errno == EACCES)
			{
				_response = ErrorResponse::getErrorResponse(ERROR_403, _configs.
				getErrorPage(ERROR_403));
				throw std::runtime_error("403 Forbidden (_checkUploadPath/EACCES)");
			}
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_500, _configs.
				getErrorPage(ERROR_500));
				throw std::runtime_error("500 Internal Server Error (uploads) 'isNotCGI'");
			}
		}
	}
	return ;
}

void	Response::_methodPost(void)
{
	try
	{
		if ((this->_req_parsed["Content-Type"] != "application/x-www-form-urlencoded")
		&& (this->_req_parsed["Content-Type"] != "text/plain")
		&& (this->_req_parsed["Content-Type"].find("multipart/form-data") == std::string::npos))
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (_methodPost/" + this->_req_parsed["Content-Type"] + ")");
		}
		else if (((_url_path.find("/cgi")) == std::string::npos)
		&& (this->_req_parsed["Content-Type"] != "application/x-www-form-urlencoded"))
			_isNotCGI();
		else if ((_url_path.find("/cgi") == std::string::npos)
		&& (this->_req_parsed["Content-Type"] == "application/x-www-form-urlencoded"))
		{
			_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
			getErrorPage(ERROR_400));
			throw std::runtime_error("400 Bad Request (_methodPost/content-type invalid whithout CGI)");
		}
		else
		{
			_checkCgiRequest();
			_checkUploadPath();
			if (this->_req_parsed["Transfer-Encoding"] == "chunked")
				_parseChunk();
			if (this->_req_parsed["Content-Type"] == "application/x-www-form-urlencoded")
				_parseUrlEncodedParams();
			else if (this->_req_parsed["Content-Type"] == "text/plain")
				_parseTextPlain();
			else if (this->_req_parsed["Content-Type"].find("multipart/form-data") != std::string::npos)
				_parseMultipartFormData();
			else
			{
				_response = ErrorResponse::getErrorResponse(ERROR_400, _configs.
				getErrorPage(ERROR_400));
				throw std::runtime_error("400 Bad Request (_methodPost/" + this->_req_parsed["Content-Type"] + ")");
			}
		}
		// post default answer
		printHeader ("200", "OK", this->_req_parsed["Version"]);
	}
	catch (std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
	}

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
	// if (!_configs.getHttpMethods().empty() && _configs.getHttpMethods().find
	// 	(_req_parsed["Method"]) == _configs.
	// 	getHttpMethods().end())
	// {
	// 	std::cerr << "Error 405 invalid method" << std::endl;
	// 	_response = ErrorResponse::getErrorResponse(ERROR_405, _configs.
	// 		getErrorPage(ERROR_405));
	// 	return (true);
	// }
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
			name = "Enter a query name='your name'";
	std::ofstream WriteFile("index/file.txt");
	WriteFile << name;
}

bool	Response::headerCheck(void)
{
	std::ostringstream ss;
	ss << _configs.getPort();
	std::string port = ss.str();
	/*Checagens de alguns Bad Requests mais comuns*/
	if (_req_parsed["Host"] != "127.0.0.1:" + port &&
		_req_parsed["Host"] != "localhost:" + port)
	{
		std::cout << _req_parsed["Host"] << std::endl;
		std::cout << "127.0.0.1:" << port << std::endl;
		_req_parsed["Version"] = "Bad Request";
		return (true);
	}
	if (_req_parsed["Content-Length"] != "")
	{
		_req_parsed["Version"] = "Bad Request";
		return (true);
	}
	if (_req_parsed[""] != "")
	{
		std::cout << _req_parsed["Host"] << std::endl;
		_req_parsed["Version"] = "Bad Request";
		return (true);
	}
	return (false);
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
	headerCheck();
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

