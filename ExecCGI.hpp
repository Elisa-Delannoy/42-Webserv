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
		bool	_wrote;
		bool	_read;
		size_t	_w_len;
		int		_count_read;
		int		_count_write;
		int		_time_begin_cgi;

	public:
		ExecCGI();
		~ExecCGI();

		int Execution(ParseRequest &header, ParseBody &body, SocketServer socket_server, Epoll& epoll);
		bool CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &server);
		void KillAndClose();
		void DeleteArgvEnvp();
		int Read(Epoll& epoll);
		int Write(ParseBody &body);

		void SetRead(bool state);
		void SetWrote(bool state);
		void SetArgv(Location &location, std::string &path);
		void SetEnvp(ParseRequest &header, ParseBody &body, std::string& path, SocketServer& socket_server);
		void SetTimeBeginCGI();
		void SetCgibody(std::string str);

		char** GetEnvp() const;
		char** GetArgv() const;
		int		GetFdOut();
		int		GetFdIn();
		bool	GetRead();
		bool	GetWrote();

		std::string GetCgiBody() const;
		int GetTimeBeginCGI() const;
};