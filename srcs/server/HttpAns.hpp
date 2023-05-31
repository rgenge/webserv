#ifndef HTTPANS_HPP
# define HTTPANS_HPP

# include "string"

class HttpAns {
	/* A intenção desta clase é servir como um repositório
	de constantes, como por exemplo as mensagens de respostas
	dos requests HTTP*/

	public:
	
		static const std::string GET_HTML;

	private:

		HttpAns() {};

};

const std::string HttpAns::GET_HTML = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 1024\nCache-Control: no-cache\n\n";

#endif /* HTTPANS_HPP */
