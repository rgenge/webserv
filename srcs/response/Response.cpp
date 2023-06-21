#include "Response.hpp"
#include <dirent.h>

Response::Response()
{
}

Response::Response(std::string res_input)
{
	parse(res_input);
}

void Response::parse(std::string res_input)
{
	std::cout << res_input;
}

/*Salva a resposta e imprimi no terminal*/
void Response::print_header(std::string status_code, std::string ok_ko)
{
	_response.append("HTTP/1.1 " + status_code + " " + ok_ko +  "\r\n");
	for (std::map<std::string, std::string>::iterator i = _res_map.begin(); i != _res_map.end(); i++)
		_response.append((*i).first + ": " + (*i).second + "\r\n");
	_response.append("\r\n");
}

/*Procura o ultimo "." do path e pega a extensao a partir dele*/
std::string Response::get_type()
{
	std::string		type;
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
	return ("text/html");
}

void Response::auto_index(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf)
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
			"<title>webserv</title></head><body><h1>Index of " + server_conf
			["Root"] + "</h1>\n");
			while ((contents = readdir(dh)) != NULL)
			{
				if (strcmp(contents->d_name, ".") || strcmp(contents->d_name,
					".."))
					_body += "<a href=\"" + std::string(contents->d_name);
				else if (_dir_path[_dir_path.length() - 1] ==
					'/')
					_body += "<a href=\"" + std::string(contents->d_name);
				else
					_body += "<a href=\"" + std::string(contents->d_name);
				_body += (contents->d_type == DT_DIR ? "/" : "") + (std::string)
					"\">";
				_body += (std::string)(contents->d_name) + (contents->d_type ==
					DT_DIR ? "/" : "") + "</a><br>";
			}
			closedir(dh);
			std::cout << map_input["Path"];
			_body += "</div></body></html>";
	}
}

void Response::method_get(std::map <std::string, std::string> map_input,
	std::map <std::string, std::string> server_conf)
{
	if (map_input["Path"] == "/")
	{
		_full_path = server_conf["Root"] + map_input["Path"] + server_conf
			["Index"];
		_dir_path = server_conf["Root"] + map_input["Path"];
	}
	else
	{
		_full_path = server_conf["Root"] + map_input["Path"];
		_dir_path = server_conf["Root"] + map_input["Path"];

	}
	std::cout <<"FULL \t:" << _full_path <<std::endl;
		std::cout <<"DIR \t:" << _full_path <<std::endl;
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
		if (server_conf["AutoIndex"] == "off")
			_body = buffer;
		if (server_conf["AutoIndex"] == "on")
			auto_index(map_input, server_conf);
		/*Converte o Content-length de size_t pra string e adiciona no map*/
		std::stringstream sizet_len;
		sizet_len <<  buffer.size();
		std::string string_len = sizet_len.str();
		if (server_conf["AutoIndex"] == "off")
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		else if (server_conf["AutoIndex"] == "on")
		{
			std::stringstream sizet_len;
			sizet_len <<  _body.size();
			std::string string_len = sizet_len.str();
			_res_map.insert(std::pair<std::string,std::string>
			("Content-Length", string_len));
		}
		/*Verifica o Content-type do arquivo*/
		_res_map["Content-type"] = get_type();
		print_header ("200", "OK");
		page.close();
	}
}

void Response::removeBreakLines(std::string &str)
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

void Response::replaceHexPercentWithAscii(std::string &params)
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

void Response::parseUrlEncodedParams(std::string params)
{
	size_t separatorPos = params.find('=');
	if (separatorPos == std::string::npos)
		throw std::runtime_error("invalid application/x-www-form-urlencoded format");
	removeBreakLines (params);
	replaceHexPercentWithAscii(params);
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
	std::map<std::string, std::string>::iterator it;
	for(it = this->_decodedParams.begin(); it != this->_decodedParams.end(); it++)
		std::cout << it->first << "=" << it->second << std::endl;
	return ;
}

void	Response::parseTextPlain(std::string &textPlain)
{
	removeBreakLines(textPlain);
	this->_textPlain = textPlain;
	return ;
}

void	Response::parseMultipartFormData(std::string &multipart)
{
	std::cout << "MultipartFormData: " << multipart << std::endl;
	return ;
}

void	Response::methodPost(std::map <std::string, std::string> map_input,
	std::map <std::string, std::string> server_conf)
{
	(void)server_conf;
	if (map_input["Content-Type"] == "application/x-www-form-urlencoded")
		parseUrlEncodedParams(map_input["ChunkBody"]);
	else if (map_input["Content-Type"] == "text/plain")
		parseTextPlain(map_input["ChunkBody"]);
	else if (map_input["Content-Type"] == "multipart/form-data")
		parseMultipartFormData(map_input["ChunkBody"]);
	// std::cout << "POST" << std::endl;
	// std::map<std::string, std::string>::iterator itr;
	// for(itr=map_input.begin();itr!=map_input.end();itr++)
	// 	std::cout << itr->first << ": " << itr->second << std::endl;
	// std::cout << "FIM DO POST" << std::endl;
	return ;
}

void Response::init(std::map <std::string, std::string> map_input, std::map
	<std::string, std::string> server_conf)
{
	if (map_input["Method"] == "GET")
		method_get(map_input, server_conf);
	else if (map_input["Method"] == "POST")
		methodPost(map_input, server_conf);
}

Response::~Response()
{
}

std::string Response::getResponse()
{
	return _response;
}

std::string Response::get_body()
{
	return _body;
}
