#ifndef CGIHANDLER_H
# define CGIHANDLER_H

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <ctime>
#include <sstream>
#include <map>

class	CgiHandler
{
	public:
		CgiHandler(void);
		CgiHandler(CgiHandler const &src);
		~CgiHandler(void);
		CgiHandler							&operator=(CgiHandler const &rhs);
		void								cgiHandler(void);
	private:
		void								_getEnv(void);
		void								_stringCopy(char *dst, const char *src);
		void								_convertEnvFormat(void);
		std::string							_generateFileName(void);
		void								_clearEnvp(void);
		void								_openPipe(void);
		void								_child(void);
		void								_parent(void);
		void								_execCgi(void);
		std::map<std::string, std::string>	_tempEnv;
		//std::string							_path;
		char								**_argv;
		char								**_envp;

		int									_pipeFd[2];
		pid_t								_pid;
};

#endif