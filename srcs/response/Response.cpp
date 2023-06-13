#include "Response.hpp"
#include "HttpAns.hpp"
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
	std::string	type_str;
	const char		*type;
	std::size_t last_dot = _full_path.find_last_of(".");
	type_str = _full_path.substr(last_dot, _full_path.size() - last_dot);
	type = type_str.c_str();
	if (strcmp(type, ".css") == 0) return "text/css";
	if (strcmp(type, ".csv") == 0) return "text/csv";
	if (strcmp(type, ".html") == 0) return "text/html";
	if (strcmp(type, ".htm") == 0) return "text/html";
	if (strcmp(type, ".shtml") == 0) return "text/html";
	if (strcmp(type, ".js") == 0) return "application/javascript";
	if (strcmp(type, ".json") == 0) return "application/json";
	if (strcmp(type, ".gif") == 0) return "image/gif";
	if (strcmp(type, ".jpeg") == 0) return "image/jpeg";
	if (strcmp(type, ".jpg") == 0) return "image/jpeg";
	if (strcmp(type, ".png") == 0) return "image/png";
	if (strcmp(type, ".svg") == 0) return "image/svg+xml";
	if (strcmp(type, ".ico") == 0) return "image/x-icon|";
	return ("");
}
void Response::method_get(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf)
{
	if (map_input["Path"] == "/")
		_full_path = server_conf["Root"] + map_input["Path"] + server_conf["Index"];
	else
		_full_path = server_conf["Root"] + map_input["Path"];
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
//		_response += buffer;
		_body = buffer;
		/*Converte o Content-length de size_t pra string e adiciona no map*/
		std::stringstream sizet_len;
		sizet_len <<  buffer.size();
		std::string string_len = sizet_len.str();
		_res_map.insert(std::pair<std::string,std::string>("Content-Length", string_len));
		std::cout <<"teste" << _res_map["Content-Length"] << "\n\n";
		/*Verifica o Content-type do arquivo*/
		_res_map["Content-type"] = get_type();
		print_header ("200", "OK");
		page.close();
	}


}
void Response::init(std::map <std::string, std::string> map_input, std::map <std::string, std::string> server_conf)
{
//	_response += HttpAns::GET_HTML;
	if (map_input["Method"] == "GET")
		method_get(map_input, server_conf);

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
