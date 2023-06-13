#include "CgiHandler.hpp"

CgiHandler::CgiHandler(void)
{
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
		;
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

std::string	CgiHandler::_generateFileName(void)
{
	static int	sequenceNumber = 0;
	std::time_t	currentTime;
	std::stringstream	fileName;

	currentTime = std::time(0);
	fileName << "file_" << currentTime << sequenceNumber++;
	return (fileName.str());
}

void	CgiHandler::_getEnv(void)
{
	// aqui pegarei as variáveis de ambiente que o script cgi pode precisar
	// talvez nem seja necessário e já venha pronto
	// por enquanto, estou criando um map temporário

	this->_tempEnv["METHOD"] = "GET";
    this->_tempEnv["HTTP_USER_AGENT"] = "Mozilla/5.0";
    this->_tempEnv["REMOTE_ADDR"] = "127.0.0.1";
    this->_tempEnv["CUSTOM_VAR1"] = "value1";
    this->_tempEnv["CUSTOM_VAR2"] = "value2";

	return ;
}

void	CgiHandler::_stringCopy(char *dst, const char *src)
{
	size_t	i = 0;
	while(src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return ;
}

void	CgiHandler::_convertEnvFormat(void)
{
	// aqui vou converter o map em um char** para que o execve possa usar no 3º argumento
	int	size = this->_tempEnv.size() + 1;
	this->_envp = new char *[size];

	int	i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = this->_tempEnv.begin(); it != this->_tempEnv.end(); it++)
	{
		std::string arg = it->first + "=" + it->second;
		size = arg.size() + 1;
		this->_envp[i] = new char[size];
		_stringCopy(this->_envp[i++], arg.c_str());
	}
	this->_envp[i] = 0;

	// for (int i = 0; this->_envp[i]; i++)
	// 	std::cout << this->_envp[i] << std::endl;

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

	// this->_path = "usr/bin/php";
	const char* path = "/usr/bin/php";
	const char* scriptPath = "./cgi_bin/script.php";
	char* execArgs[] = {
		const_cast<char*>(path),
		const_cast<char*>(scriptPath),
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
	close(this->_pipeFd[1]);
	std::ofstream file;
	file.open(_generateFileName().c_str(), std::ios::out);
	if (!file.is_open())
	{
		_clearEnvp();
		throw std::runtime_error("ofstream");
	}

	char	buffer[4096];
	ssize_t	bytesRead;
	while ((bytesRead = read(this->_pipeFd[0], buffer, sizeof(buffer))) > 0)
		file << buffer;

	int status;
	close(this->_pipeFd[0]);
	waitpid(this->_pid, &status, 0);
	file.close();
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

void	CgiHandler::cgiHandler(void)
{
	_getEnv();
	_convertEnvFormat();
	_execCgi();
	return ;
}
