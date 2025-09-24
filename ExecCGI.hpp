#pragma once

#include <string>
#include <vector>
#include <cstring>

class ExecCGI
{
	private:
		char **argv;
		char **envp;
		int pipefd[2];
		pid_t pid;
	public:
		ExecCGI();
		~ExecCGI();

		void SetArgv(std::vector<std::string> &vec);
		void SetEnvp(std::vector<std::string> &vec);
};