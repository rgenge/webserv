#include "ErrorResponse.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

ErrorResponse::~ErrorResponse() {
}

std::string	ErrorResponse::getErrorResponse(ErrorCode errorCode, std::string const &errorPage) {
	std::string	errorResponse;
	std::string	errorBody;
	size_t		bodySize;

	errorBody = ErrorResponse::_getErrorBody(errorCode, errorPage);
	bodySize = errorBody.size();
}

std::string ErrorResponse::_getErrorHeader(ErrorCode errorCode, size_t bodySize) {
	std::ostringstream	errorHeader;

	errorHeader << "HTTP/1.1 " << _getErrorMsg(errorCode) << CRLF;
	errorHeader << "Content-Type: text/html";
	errorHeader << "Content-Length: " << bodySize;
	errorHeader << CRLF << CRLF;
	return (errorHeader.str());
}

std::string	ErrorResponse::_getErrorBody(ErrorCode errorCode, std::string const &errorPage) {
	std::ifstream	errorFile;
	std::string		filePath;
	std::string		errorBody;

	if (errorPage.empty() == false)
		filePath = errorPage;
	else
		filePath = _getErrorPage(errorCode);
	errorFile.open(errorPage.c_str(), std::ios::in);
	if (errorFile.fail())
		return (DEFAULT_PAGE);
	errorBody = _readErrorBodyFile(errorFile);
	errorFile.close();
	return (errorBody);
}

std::string	ErrorResponse::_readErrorBodyFile(std::ifstream &file) {
	std::ostringstream	body;
	std::string			line;

	while (std::getline(file, line)) {
		body << line << '\n';
	}
	return (body.str());
}

std::string	ErrorResponse::_getErrorMsg(ErrorCode errorCode) {
	switch (errorCode) {
		case ERROR_400:
			return ("400 Bad Request");
		case ERROR_401:
			return ("401 Unauthorized");
		case ERROR_403:
			return ("403 Forbidden");
		case ERROR_404:
			return ("404 Not Found");
		case ERROR_405:
			return ("405 Method Not Allowed");
		case ERROR_406:
			return ("406 Not Acceptable");
		case ERROR_500:
			return ("500 Internal Server Error");
		case ERROR_502:
			return ("502 Bad Gateway");
		case ERROR_505:
			return ("505 HTTP Version Not Supported");
	}
}

std::string	ErrorResponse::_getErrorPage(ErrorCode errorCode) {
	switch (errorCode) {
		case ERROR_400:
			return ("./errorpages/400.html");
		case ERROR_401:
			return ("./errorpages/401.html");
		case ERROR_403:
			return ("./errorpages/403.html");
		case ERROR_404:
			return ("./errorpages/404.html");
		case ERROR_405:
			return ("./errorpages/405.html");
		case ERROR_406:
			return ("./errorpages/406.html");
		case ERROR_500:
			return ("./errorpages/500.html");
		case ERROR_502:
			return ("./errorpages/502.html");
		case ERROR_505:
			return ("./errorpages/505.html");
	}
}
