#include "Response.hpp"

Response::Response(std::map <std::string, std::string>& _res_param_, std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_,
ServerConfig & _configs_, std::string&_url_path_) :_res_param(_res_param_),
_req_parsed(_req_parsed_), _serverConfig (_serverConfig_), _configs(_configs_),
_url_path(_url_path_)
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
	_error_flag = 1;
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
		printHeader("301", "Moved Permanently", _req_parsed["Version"]);
		return;
	}
	std::string check_url;
	check_url = _req_parsed["Path"].substr(0, _url_path.size());
	/*Limpa o caminho e tira o url pra adicionar ao root*/
	std::string		clean_path = _req_parsed["Path"];
	std::string::size_type i = clean_path.find(_url_path);
	if (i != std::string::npos && _url_path != "/")
		clean_path.erase(i, _url_path.length());
	_full_path = _configs.getRoot() + clean_path;
	_dir_path = _configs.getRoot() + clean_path;
	/*limpa o path quando adicionar // */
	if(_full_path.find("//") != std::string::npos)
		_full_path.replace(_full_path.find("//"), 2, "/");
	/*Checa se o index existe caso exista acessa ele */
	if (access (((_full_path + + "/" + _configs.getIndex()).c_str()), F_OK) !=
		-1)
		_full_path = _full_path + "/" +  _configs.getIndex();
	/*Ativa o redirect caso possua*/
	if (_configs.getRedirect() != "")
	{
		_full_path = _configs.getRedirect();
		_dir_path = _configs.getRedirect();
	}
	/*CGI funciona mas sem verificar input do server*/
	if (_full_path.find(".php") != std::string::npos)
	{
		CgiHandler	cgi_init;
		std::string	cgi_body;
		_body = cgi_init.cgiHandler(_full_path);
		return;
	}
	std::cout <<"FULL \t:" << _full_path <<std::endl;
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
		if (_configs.getRedirect() != "")
		{
			printHeader ("308", "Permanent Redirect", _req_parsed["Version"]);
			return ;
		}
		if (_error_flag != 1)
			printHeader ("200", "OK", _req_parsed["Version"]);
		page.close();
	}
	else
		printError("404", "Not Found");
}

void	Response::methodDelete(std::map <std::string, std::string> _req_parsed)
{
	std::string		clean_path = _req_parsed["Path"];
	std::string::size_type i = clean_path.find(_url_path);
	if (i != std::string::npos && _url_path != "/")
		clean_path.erase(i, _url_path.length());
	_full_path = _configs.getRoot() + clean_path;
	_dir_path = _configs.getRoot() + clean_path;
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
	if (_configs.getHttpMethods().find(_req_parsed["Method"]) == _configs.
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
	_error_flag = 0;
	/*Iniciando o server com os dados do path selecionado*/
	if (_req_parsed["Path"] == "/")
	{
		_configs = ServerConfig(_serverConfig);
		_url_path = _req_parsed["Path"];
	}
	else if (_serverConfig.routes.find(_req_parsed["Path"]) == _serverConfig.
		routes.end())
		std::cerr << "";
	else
	{
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[_req_parsed
			["Path"]]);
		_url_path = _req_parsed["Path"];
	}
	// std::cout <<"Root:"<< _configs.getRoot() << std::endl;
	// std::cout <<"Auxindex:"<< _configs.getDirList() << std::endl;
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
