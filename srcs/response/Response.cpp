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

void Response::auto_index(std::map <std::string, std::string> _req_parsed, std::map <std::string, std::string> _res_param)
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
			std::cout << _req_parsed["Path"];
			_body += "</div></body></html>";
	}
}

void Response::method_get(std::map <std::string, std::string> _req_parsed,
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
		if (_res_param["AutoIndex"] == "off")
			_body = buffer;
		if (_res_param["AutoIndex"] == "on")
			auto_index(_req_parsed, _res_param);
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
		_res_map["Content-type"] = get_type();
		if(_res_map["Content-Length"] > _res_param["bodySizeLimit"])
			std::cout << "error 414 bodySize Limite excedido";
		print_header ("200", "OK");
		page.close();
	}
	else
		std::cout << "Some error ";
}

void Response::method_delete(std::map <std::string, std::string> _req_parsed,
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
		print_header ("200", "OK");
	}

}
void Response::init(std::map <std::string, std::string> _req_parsed, std::map
	<std::string, std::string> _res_param)
{
	if (_req_parsed["Method"] == "GET")
		method_get(_req_parsed, _res_param);
	if (_req_parsed["Method"] == "DELETE")
		method_delete(_req_parsed, _res_param);
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
