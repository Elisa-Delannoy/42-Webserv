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

class ExecCGI
{
	private:
		char **_argv;
		char **_envp;
	public:
		ExecCGI();
		~ExecCGI();

		std::string Execution(ParseRequest &header, ParseBody &body, Location &location, std::string &ext);
		int CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers);

		void SetArgv(Location &location, std::string &ext);
		void SetEnvp(ParseRequest &header, ParseBody &body, std::string path);

		char** GetEnvp() const;
		char** GetArgv() const;
};