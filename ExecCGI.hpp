#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <ServerConf.hpp>
#include <ParseRequest.hpp>

class ExecCGI
{
	private:
		char **argv;
		char **envp;
	public:
		ExecCGI();
		~ExecCGI();

		void Execution(std::vector<ServerConf> servers);

		void SetArgv(std::vector<std::string> &vec);
		void SetEnvp(std::vector<std::string> &vec);
};