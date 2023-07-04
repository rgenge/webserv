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
		std::string							cgiHandler(std::string path);
	private:
		void								_getEnv(void);
		void								_convertEnvFormat(void);
		void								_clearEnvp(void);
		void								_openPipe(void);
		void								_child(void);
		void								_parent(void);
		void								_execCgi(void);

		std::map<std::string, std::string>	_tempEnv;
		std::string							_cgiResult;
		std::string							_path;
		pid_t								_pid;
		char								**_envp;
		int									_pipeFd[2];
};

#endif
