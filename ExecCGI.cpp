#include "ExecCGI.hpp"

ExecCGI::ExecCGI()
{}

ExecCGI::~ExecCGI()
{}

void ExecCGI::SetEnvp(ParseRequest header, ParseBody body, Location location)
{
	std::vector<std::string> env;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + header.GetMethod());
	env.push_back("SCRIPT_FILENAME=" + location.GetRoot() + header.GetPath());
	env.push_back("SERVER_PROTOCOL=" + header.GetVersion());
	env.push_back("REDIRECT_STATUS=200");
	if (header.GetMethod() == "POST")
	{
		std::ostringstream oss;
		oss << body.GetContentLen();
		std::string len = oss.str();
		env.push_back("CONTENT_LENGTH=" + len);
		env.push_back("CONTENT_TYPE=" + body.GetContentType());
	}
	_envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
	{
		_envp[i] = new char[env[i].size() + 1];
		std::strcpy(_envp[i], env[i].c_str());
	}
	_envp[env.size()] = NULL;
}

void ExecCGI::SetArgv(std::string path, Location location)
{
	std::vector<std::string> argv;
	argv.push_back(location.GetCGIPass());
	argv.push_back(location.GetRoot() + path);
	_argv = new char*[argv.size() + 1];
	for (size_t i = 0; i < argv.size(); i++)
	{
		_argv[i] = new char[argv[i].size() + 1];
		std::strcpy(_argv[i], argv[i].c_str());
	}
	_argv[argv.size()] = NULL;
}

bool ExecCGI::CheckCGI(ParseRequest header, ParseBody body, std::vector<ServerConf> servers) /*reference ?*/
{
	if (header.GetPath().find(".php") != std::string::npos)
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			int nb = servers[i].checkLocation(".php");
			if (nb >= 0 && servers[i].checkMethods(header.GetMethod(), nb) == true)
			{
				Execution(header, body, servers[i].GetLocation(nb));
				return true;	
			}	
		}
		return false;
	}
	return false;
}

void ExecCGI::Execution(ParseRequest header, ParseBody body, Location location)
{
	SetArgv(header.GetPath(), location);
	SetEnvp(header, body, location);
	for (int i = 0; _envp[i] != NULL; i++)
	{
		std::cout << _envp[i] << std::endl;
	}
}