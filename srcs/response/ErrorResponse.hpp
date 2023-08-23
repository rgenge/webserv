#ifndef ERRORRESPONSE_HPP
# define ERRORRESPONSE_HPP

# include <string>
# define CRLF "\r\n"
# define DEFAULT_PAGE "<!DOCTYPE html> \
</head> \
<body style=\"text-align: center;\"> \
	<h1>Sorry, there's no error page to display :c</h1> \
	<p>Webserver thinks someone has stolen the default error page!</p> \
</body> \
</html>"

enum ErrorCode {
	ERROR_400,
	ERROR_401,
	ERROR_403,
	ERROR_404,
	ERROR_405,
	ERROR_406,
	ERROR_411,
	ERROR_413,
	ERROR_500,
	ERROR_501,
	ERROR_502,
	ERROR_505,
};

class ErrorResponse {

	public:

		~ErrorResponse();

		static std::string	getErrorResponse(ErrorCode errorCode, std::string const &errorPage);

	private:

		ErrorResponse();

		static std::string	_getErrorBody(ErrorCode errorCode, std::string const &errorPage);
		static std::string	_getErrorHeader(ErrorCode errorCode, size_t bodySize);
		static std::string	_getErrorMsg(ErrorCode errorCode);
		static std::string	_getErrorPage(ErrorCode errorCode);
		static std::string	_readErrorBodyFile(std::ifstream &file);
};

#endif /* ERRORRESPONSE_HPP */
