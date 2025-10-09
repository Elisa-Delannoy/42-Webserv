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
	env.push_back("SCRIPT_NAME=" + header.GetPath());
	env.push_back("SERVER_PROTOCOL=" + header.GetVersion());
	env.push_back("REDIRECT_STATUS=200");
	size_t pos = header.GetPath().find('?');
	if (pos != std::string::npos)
		env.push_back("QUERY_STRING=" + header.GetPath().substr(pos + 1));
	else
	{
		env.push_back("QUERY_STRING=");
	}
	env.push_back("PATH_INFO=");
	env.push_back("PATH_TRANSLATED=");
	if (header.GetMethod() == "POST")
	{
		std::ostringstream oss;
		oss << body.GetContentLen();
		std::string len = oss.str();
		env.push_back("CONTENT_LENGTH=" + len);
		env.push_back("CONTENT_TYPE=" + body.GetContentType());
	}
	else
	{
		env.push_back("CONTENT_LENGTH=0");
		env.push_back("CONTENT_TYPE=");
	}
	_envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
	{
		_envp[i] = new char[env[i].size() + 1];
		std::strcpy(_envp[i], env[i].c_str());
	}
	_envp[env.size()] = NULL;
}


void ExecCGI::SetArgv(Location &location, std::string &ext)
{
	std::vector<std::string> argv;
	argv.push_back(location.GetCGIPass(ext));
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
		pos = newpath.find('?');
		if (pos != std::string::npos)
		{
			newpath = newpath.substr(0, pos);
		}
		return newpath;
	}

std::string ExecCGI::Execution(ParseRequest &header, ParseBody& body, Location &location, std::string &ext)
{
	std::cout << "\n========== DEBUT EXECUTION CGI ==========" << std::endl;
	
	std::string path = SetupPath(header.GetPath(), location.GetName(), location.GetRoot());
	// char resolved[PATH_MAX];
	// if (realpath(path.c_str(), resolved) == NULL)
	// {
	// 	std::cerr << "[ERROR] realpath failed: " << strerror(errno) << std::endl;
	// 	std::cerr << "[ERROR] Path: " << path << std::endl;
	// 	return "";
	// }
	// path = std::string(resolved);
	// if (access(path.c_str(), R_OK) != 0)
	// {
	// 	std::cerr << "[ERROR] File not readable: " << strerror(errno) << std::endl;
	// 	return "";
	// }
	
	SetArgv(location, ext);
	SetEnvp(header, body, path);
	
	int pipe_in[2];
	int pipe_out[2];
	
	if (pipe(pipe_in) == -1)
	{
		std::cerr << "[ERROR] pipe_in failed: " << strerror(errno) << std::endl;
		return "";
	}
	if (pipe(pipe_out) == -1)
	{
		std::cerr << "[ERROR] pipe_out failed: " << strerror(errno) << std::endl;
		return "";
	}
	
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "[ERROR] fork failed: " << strerror(errno) << std::endl;
		return "";
	}
	
	if (pid == 0)
	{
		close(pipe_in[1]);
		close(pipe_out[0]);
		
		if (dup2(pipe_in[0], STDIN_FILENO) == -1)
		{
			std::cerr << "[CHILD ERROR] dup2 stdin: " << strerror(errno) << std::endl;
			return "";
		}
		if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "[CHILD ERROR] dup2 stdout: " << strerror(errno) << std::endl;
			return "";
		}
		// if (dup2(pipe_out[1], STDERR_FILENO) == -1)
		// {
		// 	std::cerr << "[CHILD ERROR] dup2 stderr: " << strerror(errno) << std::endl;
		// 	return "";
		// }
		
		close(pipe_in[0]);
		close(pipe_out[1]);

		execve(location.GetCGIPass(ext).c_str(), GetArgv(), GetEnvp());

		std::cerr << "[CHILD ERROR] Command was: " << location.GetCGIPass(ext) << std::endl;
		exit(1);
	}
	else
	{
		close(pipe_in[0]);
		close(pipe_out[1]);

		std::string bodyContent = body.GetBody();
		if (!bodyContent.empty())
		{
			ssize_t written = write(pipe_in[1], bodyContent.c_str(), bodyContent.length());
			if (written == -1)
				std::cerr << "[PARENT ERROR] write failed: " << strerror(errno) << std::endl;
		}
		
		close(pipe_in[1]);
		
		usleep(100000);

		fd_set readfds;
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		FD_ZERO(&readfds);
		FD_SET(pipe_out[0], &readfds);

		int select_ret = select(pipe_out[0] + 1, &readfds, NULL, NULL, &timeout);
		
		if (select_ret == -1)
		{
			std::cerr << "[PARENT ERROR] select failed: " << strerror(errno) << std::endl;
			close(pipe_out[0]);
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			return "";
		}
		else if (select_ret == 0)
		{
			std::cerr << "[PARENT ERROR] Timeout! No output from CGI" << std::endl;
			close(pipe_out[0]);
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			return "";
		}
		
		std::string cgihtml;
		char buffer[4096];
		ssize_t bytesRead;
		
		while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			cgihtml += buffer;
		}
		if (bytesRead == -1)
			std::cerr << "[PARENT ERROR] read failed: " << strerror(errno) << std::endl;
		
		close(pipe_out[0]);
		
		waitpid(pid, NULL, 0);
		
		return cgihtml;
	}
}


int ExecCGI::CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers) /*reference ?*/
{
	(void)body;
	(void)servers;
	size_t pos = header.GetPath().find(".");
	if (pos != std::string::npos)
	{
		std::string ext = header.GetPath().substr(pos);
		size_t sep = ext.find("?");
		if (sep != std::string::npos)
			ext = ext.substr(0, sep);
		Location loc;
		if (servers.HasLocationForExtension(header.GetNameLocation(), ext, loc))
		{
			std::string result;
			result = Execution(header, body, loc, ext);
			std::cout << "result = " << result << std::endl;
			return 0;
		}
	}
	return 1;
}
