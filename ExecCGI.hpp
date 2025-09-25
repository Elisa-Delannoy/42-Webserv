#pragma once

#include <string>
#include <vector>
#include <cstring>
#include "ServerConf.hpp"
#include "ParseRequest.hpp"
#include "ParseBodyRequest.hpp"

class ExecCGI
{
	private:
		char **_argv;
		char **_envp;
	public:
		ExecCGI();
		~ExecCGI();

		void Execution(ParseRequest header, ParseBody body, Location location);
		bool CheckCGI(ParseRequest header, ParseBody body, std::vector<ServerConf> servers);

		void SetArgv(std::string path, Location location);
		void SetEnvp(ParseRequest header, ParseBody body, Location location);
};