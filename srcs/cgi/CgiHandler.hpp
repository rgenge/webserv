#ifndef CGIHANDLER_H
# define CGIHANDLER_H

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ErrorResponse.hpp"
#include "ServerConfig.hpp"
#include <map>

class	CgiHandler
{
	public:
		CgiHandler(std::string bodyPath, std::string &scriptPath,
		std::map<std::string, std::string> &envpMap, std::string &response,
		ServerConfig &_configs);
		CgiHandler(CgiHandler const &src);
		~CgiHandler(void);
		CgiHandler							&operator=(CgiHandler const &rhs);
		std::string							cgiHandler(void);
	private:
		void								_convertEnvFormat(void);
		void								_clearEnvp(void);
		void								_openPipe(void);
		void								_child(void);
		void								_parent(void);
		void								_execCgi(void);

		ServerConfig						&_configs;
		std::map<std::string, std::string>	&_envpMap;
		std::string							_cgiResult;
		std::string							_bodyPath;
		std::string							&_scriptPath;
		std::string							&_response;
		pid_t								_pid;
		char								**_envp;
		int									_pipeFd[2];
};

#endif
