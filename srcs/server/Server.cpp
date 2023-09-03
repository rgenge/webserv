#include "Server.hpp"
#include "ErrorResponse.hpp"

std::map<int, std::vector<unsigned char> >	Server::requestComplete;
bool										Server::endChunk = false;
bool										Server::isChunk = false;
bool										Server::firstChunk = true;

Server::Server(t_serverConfig const &config) : Socket(10, config.port), _serverConfig(config) {
}

Server::~Server() {
}

Server::Server(Server const &rhs) : Socket(10, rhs._port) {
	*this = rhs;
}

Server&	Server::operator=(Server const &rhs) {
	if (this == &rhs)
		return (*this);
	Socket::operator=(rhs);
	this->_requestfds = rhs._requestfds;
	this->_serverConfig = rhs._serverConfig;
	return (*this);
}

void	Server::addRequestfd(int requestfd, std::vector<unsigned char> requestMessage) {
	_requestfds.insert(std::pair<int, std::vector<unsigned char> >(requestfd, requestMessage));
}

size_t	Server::_findSequenceVector(std::vector<unsigned char> const &vector, std::string const sequence)
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

requestStatus	Server::_checkRequestStatus(std::vector<unsigned char> const &_request) {
	size_t						contentLengthIndex;
	size_t						contentLength;
	size_t						bodyPosition;
	std::string					contentValue;
	std::vector<unsigned char>	body;
	int							endHeaderDelimiter = 4;

	contentLengthIndex = _findSequenceVector(_request, "Content-Length: ");
	if (contentLengthIndex == std::string::npos)
		return (DONE);
	contentLengthIndex += 16;
	for (size_t i = contentLengthIndex; std::isdigit(static_cast<int>(_request[i])); i++)
		contentValue += _request[i];
	contentLength = std::atoi(contentValue.c_str());
	bodyPosition = _findSequenceVector(_request, DCRLF) + endHeaderDelimiter;
	for (size_t i = bodyPosition; i < _request.size(); i++)
		body.push_back(_request[i]);
	if (body.size() != contentLength)
		return (PROCESSING);
	return (DONE);
}

requestStatus	Server::_parseChunk(int requestfd)
{
	size_t				npos;

	isChunk = true;
	if (((npos = _findSequenceVector(this->_requestfds[requestfd], "0\r\n\r\n")) != std::string::npos)
	&& (npos + 5 == this->_requestfds[requestfd].size()))
	{
		endChunk = true;
		this->_requestfds[requestfd].erase(this->_requestfds[requestfd].begin() + npos,
		this->_requestfds[requestfd].begin() + npos + 1);
	}
	if (_findSequenceVector(this->_requestfds[requestfd], "Transfer-Encoding: chunked") != std::string::npos)
	{
		if ((npos = _findSequenceVector(this->_requestfds[requestfd], DCRLF)) != std::string::npos)
		{
			// copy headers
			for (size_t i = 0; i < (npos + 4); i++)
			{
				this->requestComplete[requestfd].push_back(this->_requestfds[requestfd][0]);
				this->_requestfds[requestfd].erase(this->_requestfds[requestfd].begin(),
				this->_requestfds[requestfd].begin() + 1);
			}
			if ((npos = _findSequenceVector(this->requestComplete[requestfd], "Expect: 100-continue")) != std::string::npos)
			{
				this->_requestfds[requestfd].clear();
				std::string	responseChunk = "HTTP/1.1 100 Continue\r\n\r\n";
				write(requestfd, responseChunk.c_str(), responseChunk.length());
				return (PROCESSING);
			}
		}
		else
		{
			this->_requestfds[requestfd].clear();
			return (ERROR);
		}
	}
	if ((npos = _findSequenceVector(this->_requestfds[requestfd], "\r\n")) != std::string::npos)
	{
		if (firstChunk == true)
		{
			this->_requestfds[requestfd].erase(this->_requestfds[requestfd].begin(),
			this->_requestfds[requestfd].begin() + npos + 2);
			firstChunk = false;
		}
		else
		{
			this->_requestfds[requestfd].erase(this->_requestfds[requestfd].begin() + npos,
			this->_requestfds[requestfd].begin() + npos + 2);
			size_t npos2;
			if ((npos2 = _findSequenceVector(this->_requestfds[requestfd], "\r\n")) != std::string::npos)
				this->_requestfds[requestfd].erase(this->_requestfds[requestfd].begin() + npos,
				this->_requestfds[requestfd].begin() + npos2 + 2);
		}
	}
	for (size_t i = 0; i < this->_requestfds[requestfd].size(); i++)
		this->requestComplete[requestfd].push_back(this->_requestfds[requestfd][i]);
	this->_requestfds[requestfd].clear();
	if (endChunk == true)
		return (DONE);
	return (PROCESSING);
}

requestStatus	Server::getRequest(int requestfd) {
	int		bytesRead;
	char	_request[10000] = {0};

	bytesRead = read(requestfd, _request, 8000);
	if (bytesRead < 0) {
		_requestfds.erase(requestfd);
		requestComplete.erase(requestfd);
		_respondInternalServerError(requestfd);
		close(requestfd);
		return (ERROR);
	}
	if (bytesRead == 0) {
		_requestfds.erase(requestfd);
		requestComplete.erase(requestfd);
		_respondInternalServerError(requestfd); // check
		close(requestfd);
		return (ERROR);
	}
	else
	{
		for (int i = 0; i < bytesRead; i++)
			this->_requestfds[requestfd].push_back(static_cast<unsigned char>(_request[i]));
		if ((isChunk == true)
		|| ((_findSequenceVector(this->_requestfds[requestfd], "Transfer-Encoding: chunked") != std::string::npos)))
		{
			enum requestStatus status = _parseChunk(requestfd);
			if (status == ERROR)
			{
				_requestfds.erase(requestfd);
				requestComplete.erase(requestfd);
				_respondInternalServerError(requestfd);
				close(requestfd);
				return (status);
			}
			else
			{
				_requestfds[requestfd].clear();
				return (status);
			}
		}
		else
		{
			for (size_t i = 0; i < _requestfds[requestfd].size(); i++)
				requestComplete[requestfd].push_back(_requestfds[requestfd][i]);
			if (_findSequenceVector(_requestfds[requestfd], "Expect: 100-continue") != std::string::npos)
			{
				std::string	responseChunk = "HTTP/1.1 100 Continue\r\n\r\n";
				write(requestfd, responseChunk.c_str(), responseChunk.length());
				_requestfds[requestfd].clear();
				return (PROCESSING);
			}
			else
			{
				_requestfds[requestfd].clear();
				endChunk = true;
			}
		}
	}
	return (_checkRequestStatus(requestComplete[requestfd]));
}

void	Server::respondRequest(int requestfd) {
	if (endChunk == true)
	{
		// for (size_t i = 0; i < requestComplete[requestfd].size(); i++)
		// 	std::cout << requestComplete[requestfd][i];
		// std::cout << "\nsize: " << requestComplete[requestfd].size() << std::endl;
		std::string	response;
		/*Parseamento do request e salva um map comtudo e o body do request*/
		Request _req(requestComplete[requestfd]);
		/*Iniciando o response*/
		Response res_struct(_req.getMap(), _serverConfig,
			_url_path, _req.getStrBody(), _req.getVectorBody(), _actual_root);
		res_struct.init();
		/*response recebe o header e body da resposta e escreve no fd*/
		response = res_struct.getResponse();
		write(requestfd, response.c_str(), response.length());
		_requestfds.erase(requestfd);
		requestComplete.erase(requestfd);
		close(requestfd);
	}
}

void	Server::_respondInternalServerError(int requestfd) {
	std::string	_response;

	_response = ErrorResponse::getErrorResponse(ERROR_500, _serverConfig.errorPages[500]);
	write(requestfd, _response.c_str(), _response.size());
}

bool	Server::hasRequestFd(int requestfd) {
	if (this->_requestfds.find(requestfd) != this->_requestfds.end())
		return (true);
	return (false);
}
