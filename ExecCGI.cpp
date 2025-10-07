#include "ExecCGI.hpp"

ExecCGI::ExecCGI()
{}

ExecCGI::~ExecCGI()
{}

char** ExecCGI::GetEnvp() const
{
	return _envp;
}

char** ExecCGI::GetArgv() const
{
	return _argv;
}

std::string replace_substring(std::string str, const std::string& to_replace, const std::string& replacement) 
{
	std::string::size_type pos = str.find(to_replace);
	if (pos != std::string::npos)
	{
		str.replace(pos, to_replace.length(), replacement);
	}
	return str;
}

void ExecCGI::SetEnvp(ParseRequest &header, ParseBody &body, std::string path)
{
	std::vector<std::string> env;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + header.GetMethod());
	env.push_back("SCRIPT_FILENAME=" + path);
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


void ExecCGI::SetArgv(std::string &path, Location &location, std::string &ext)
{
	std::vector<std::string> argv;
	argv.push_back(location.GetCGIPass(ext));
	argv.push_back(path);
	_argv = new char*[argv.size() + 1];
	for (size_t i = 0; i < argv.size(); i++)
	{
		_argv[i] = new char[argv[i].size() + 1];
		std::strcpy(_argv[i], argv[i].c_str());
	}
	_argv[argv.size()] = NULL;
}

std::string SetupPath(std::string path, const std::string& LocName, const std::string& LocRoot)
{
	std::string newpath = path;
	size_t pos = newpath.find('/');
	if (pos != std::string::npos)
	{
		if (LocName == "/")
		newpath.replace(pos, LocName.length() - 1, LocRoot);
		else 
			newpath.replace(pos, LocName.length(), LocRoot);
		}
		newpath = newpath.substr(1);
		return newpath;
	}

std::string ExecCGI::Execution(ParseRequest &header, ParseBody& body, Location &location, std::string &ext)
{
	std::string path = SetupPath(header.GetPath(), location.GetName(), location.GetRoot());
	SetArgv(path, location, ext);
	SetEnvp(header, body, path);
	int pipefd[2];
	if (pipe(pipefd) == -1)
		std::cerr << "Error creation pipe." << std::endl;
	pid_t pid = fork();
	if (pid == -1)
		std::cerr << "Error creation child." << std::endl;
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDIN_FILENO);
		close(pipefd[1]);
		execve(location.GetCGIPass(ext).c_str(), GetArgv(), GetEnvp());
		std::cerr << "Error execve." << std::endl;
		return "";
	}
	else 
	{
		close(pipefd[1]);
		char buffer[4096];
		ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
		if (bytesRead > 0)
			buffer[bytesRead] = '\0';
		std::string cgihtml = buffer;
		close(pipefd[0]);
		waitpid(pid, NULL, 0);
		if (cgihtml.empty())
			return "";
		return cgihtml;
	}
}

std::string ExecCGI::CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers) /*reference ?*/
{
	(void)body;
	(void)servers;
	size_t pos = header.GetPath().find(".");
	if (pos != std::string::npos)
	{
		std::string ext = header.GetPath().substr(pos, header.GetPath().length() - 1);
		std::cout << ext << std::endl;
		size_t sep = ext.find("?");
		if (sep != std::string::npos)
			ext = ext.substr(0, sep);
		Location loc;
		if (servers.HasLocationForExtension(header.GetNameLocation(), ext, loc))
		{
			std::cout << loc.GetName() << std::endl;
			std::string result;
			result = Execution(header, body, loc, ext).empty();
			return result;
		}
	}
	return "";
}
