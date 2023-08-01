#ifndef CGIHANDLER_H
# define CGIHANDLER_H

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <map>

class	CgiHandler
{
	public:
		CgiHandler(void);
		CgiHandler(CgiHandler const &src);
		~CgiHandler(void);
		CgiHandler							&operator=(CgiHandler const &rhs);
		std::string							cgiHandler(std::string const filename, std::string path, std::map<std::string, std::string> &envpMap);
	private:
		void								_convertEnvFormat(void);
		void								_clearEnvp(void);
		void								_openPipe(void);
		void								_child(void);
		void								_parent(void);
		void								_execCgi(void);

		std::map<std::string, std::string>	_envpMap;
		std::string							_cgiResult;
		std::string							_bodyPath;
		std::string							_scriptPath;
		pid_t								_pid;
		char								**_envp;
		int									_pipeFd[2];
};

#endif
