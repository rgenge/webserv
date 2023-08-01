#include "CgiHandler.hpp"

CgiHandler::CgiHandler(void)
{
	_envp = NULL;
    _pipeFd[0] = 0;
    _pipeFd[1] = 0;
    _pid = 0;
	this->_cgiResult = "";
	return ;
}

CgiHandler::CgiHandler(CgiHandler const &src)
{
	*this = src;
	return ;
}

CgiHandler::~CgiHandler(void)
{
	return ;
}

CgiHandler	&CgiHandler::operator=(CgiHandler const &rhs)
{
	if (this != &rhs)
	{
		this->_envp = rhs._envp;
		this->_pipeFd[0] = rhs._pipeFd[0];
		this->_pipeFd[1] = rhs._pipeFd[1];
		this->_pid = rhs._pid;
		this->_cgiResult = rhs._cgiResult;
	}
	return (*this);
}

void	CgiHandler::_clearEnvp(void)
{
	if (this->_envp != NULL)
	{
		for (int i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
		this->_envp = NULL;
	}
	return ;
}

void	CgiHandler::_convertEnvFormat(void)
{
	// aqui vou converter o map em um char** para que o execve possa usar no 3º argumento
	int	size = this->_envpMap.size() + 1;
	this->_envp = new char *[size];

	int	i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = this->_envpMap.begin(); it != this->_envpMap.end(); it++)
	{
		std::string arg = it->first + "=" + it->second;
		size = arg.size() + 1;
		this->_envp[i] = new char[size];
		strcpy(this->_envp[i++], arg.c_str());
	}
	this->_envp[i] = NULL;

	return ;
}

void	CgiHandler::_openPipe(void)
{
	if (pipe(this->_pipeFd) == -1)
	{
		_clearEnvp();
		throw std::runtime_error("pipe function");
	}
	return ;
}

void	CgiHandler::_child(void)
{
	close(this->_pipeFd[0]);
	dup2(this->_pipeFd[1], STDOUT_FILENO);

	const char	*path = "/usr/bin/php";
	const char	*scriptPath = _scriptPath.c_str();
	char		*execArgs[] = {
		const_cast<char *>(path),
		const_cast<char *>(scriptPath),
		NULL
	};

	if (access(path, X_OK) == -1)
	{
		close(this->_pipeFd[1]);
		_clearEnvp();
		throw std::runtime_error("failed to execute interpreter");
	}

	if (access(scriptPath, X_OK) == -1)
	{
		close(this->_pipeFd[1]);
		_clearEnvp();
		throw std::runtime_error("failed to execute script CGI");
	}

	if (execve(path, execArgs, this->_envp) == -1)
	{
		close(this->_pipeFd[1]);
		_clearEnvp();
		throw std::runtime_error("execve function");
	}
	close(this->_pipeFd[1]);
	return ;
}

void	CgiHandler::_parent(void)
{
	int		status;
	ssize_t	bytesRead;
	char	buffer[4096] = {0};

	close(this->_pipeFd[1]);
	while ((bytesRead = read(this->_pipeFd[0], buffer, sizeof(buffer))) > 0)
		this->_cgiResult += buffer;
	close(this->_pipeFd[0]);
	waitpid(this->_pid, &status, 0);
	return ;
}

void	CgiHandler::_execCgi(void)
{
	_openPipe();
	this->_pid = fork();
	if (this->_pid == -1)
	{
		_clearEnvp();
		throw std::runtime_error("fork function");
	}
	else if (this->_pid == 0)
		_child();
	else
		_parent();
	return ;
}

std::string	CgiHandler::cgiHandler(std::string const bodyPath, std::string scriptPath, std::map<std::string, std::string> &envpMap)
{
	_scriptPath = scriptPath;
	_bodyPath = bodyPath;
	this->_envpMap = envpMap;
	this->_envpMap["Filename"] = this->_bodyPath;
	// std::map<std::string, std::string>::iterator	it;
	// for (it = _envpMap.begin(); it != _envpMap.end(); it++)
	// 	std::cout << it->first << it->second << std::endl;
	_convertEnvFormat();
	_execCgi();
	return (_cgiResult);
}
