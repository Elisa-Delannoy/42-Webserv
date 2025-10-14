#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ServerConf.hpp"
#include "ParseRequest.hpp"
#include "ParseBody.hpp"
#include <limits.h>  
#include <stdlib.h>
#include "Epoll.hpp"

class ExecCGI
{
	private:
		char **_argv;
		char **_envp;
		Location _loc;
		std::string _ext;
		std::string _cgibody;

		pid_t	_pid;
		int		_fdin;
		int		_fdout;
		bool 	_wrote;
		int		_time_begin_cgi;

	public:
		ExecCGI();
		~ExecCGI();

		int Execution(ParseRequest &header, ParseBody &body, Epoll& epoll);
		bool CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers);
		int ReadWrite(ParseBody &body);
		void KillAndClose();
		void DeleteArgvEnvp();

		void SetArgv(Location &location, std::string &ext);
		void SetEnvp(ParseRequest &header, ParseBody &body, std::string path);
		void SetTimeBeginCGI();
		void SetCgibody(std::string str);

		char** GetEnvp() const;
		char** GetArgv() const;
		std::string GetCgiBody() const;
		int GetTimeBeginCGI() const;
};