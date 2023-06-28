#include "Response.hpp"

Response::Response(std::map <std::string, std::string>& _res_param_, std::map
<std::string, std::string>& _req_parsed_, t_serverConfig& _serverConfig_, std::string& _actual_root_)
:_res_param(_res_param_), _req_parsed(_req_parsed_), _serverConfig
(_serverConfig_), _actual_root(_actual_root_)
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
//	if (_req_parsed["Path"] == "/")
//	{
//		_full_path = _res_param["Root"] + _req_parsed["Path"] + _res_param
	//		["Index"];
//		_dir_path = _res_param["Root"] + _req_parsed["Path"];
//	}
	// else if (_index_flag == 1)
	// {
	// 	_full_path = _res_param["Root"] + _res_param["Index"];
	// 	_dir_path = _res_param["Root"];
	// }
	if (_path_flag == 1)
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
		{
			_actual_root = _serverConfig.root;
//			_req_parsed["Path"] = "/" + _serverConfig.index;
//			_res_param["AutoIndex"] ="off";
		}
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

void	Response::init(int _flag)
{
	if (_flag == 1)
	{
	 	_actual_root = _serverConfig.root;
	 	_flag = 1;
	}
	if (req_parsed["Path"] == "/")
		_configs = ServerConfig(_serverConfig);
	else if (_serverConfig.routes.find(req_parsed["Path"]) == _serverConfig.routes.end())
	{
		std::cerr << "Error 404" : std::endl;
	}
	else
		_configs = ServerConfig(_serverConfig, _serverConfig.routes[req_parsed["Path"]]);

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
