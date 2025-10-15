#include "ExecCGI.hpp"

ExecCGI::ExecCGI() : _wrote(false), _time_begin_cgi(0)
{}

ExecCGI::~ExecCGI()
{
}

char** ExecCGI::GetEnvp() const
{
	return _envp;
}

char** ExecCGI::GetArgv() const
{
	return _argv;
}

std::string ExecCGI::GetCgiBody() const
{
	return _cgibody;
}

int ExecCGI::GetTimeBeginCGI() const
{
	return _time_begin_cgi;
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

void ExecCGI::SetEnvp(ParseRequest &header, ParseBody &body, std::string& path, SocketServer& socket_server)
{
	std::vector<std::string> env;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + header.GetMethod());
	env.push_back("SCRIPT_FILENAME=" + path);
	env.push_back("SCRIPT_NAME=" + header.GetPath());
	env.push_back("SERVER_PROTOCOL=" + header.GetVersion());
	env.push_back("SERVER_SOFTWARE=WebServer/1.0");
	env.push_back("REDIRECT_STATUS=200");
	if (socket_server.GetPort() > 0)
	{
		std::ostringstream oss;
		oss << socket_server.GetPort();
		std::string port = oss.str();
		env.push_back("SERVER_PORT=" + port);
		env.push_back("REMOTE_ADDR=" + socket_server.GetHost());
	}
	size_t pos = header.GetPath().find('?');
	if (pos != std::string::npos)
		env.push_back("QUERY_STRING=" + header.GetPath().substr(pos + 1));
	else
		env.push_back("QUERY_STRING=");
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
		env.push_back("CONTENT_LENGTH=");
		env.push_back("CONTENT_TYPE=");
	}
	
	if (_ext == ".py")
		env.push_back("PYTHONUNBUFFERED=1");
	
	_envp = new char*[env.size() + 1];
	for (size_t i = 0; i < env.size(); i++)
	{
		_envp[i] = new char[env[i].size() + 1];
		std::strcpy(_envp[i], env[i].c_str());
	}
	_envp[env.size()] = NULL;
}

// void ExecCGI::SetEnvp(ParseRequest &header, ParseBody &body, std::string path)
// {
// 	std::vector<std::string> env;

// 	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
// 	env.push_back("REQUEST_METHOD=" + header.GetMethod());
// 	env.push_back("SCRIPT_FILENAME=" + path);
// 	env.push_back("SCRIPT_NAME=" + header.GetPath());
// 	env.push_back("SERVER_PROTOCOL=" + header.GetVersion());
// 	env.push_back("REDIRECT_STATUS=200");
// 	size_t pos = header.GetPath().find('?');
// 	if (pos != std::string::npos)
// 		env.push_back("QUERY_STRING=" + header.GetPath().substr(pos + 1));
// 	else
// 	{
// 		env.push_back("QUERY_STRING=");
// 	}
// 	env.push_back("PATH_INFO=");
// 	env.push_back("PATH_TRANSLATED=");
// 	if (header.GetMethod() == "POST")
// 	{
// 		std::ostringstream oss;
// 		oss << body.GetContentLen();
// 		std::string len = oss.str();
// 		env.push_back("CONTENT_LENGTH=" + len);
// 		env.push_back("CONTENT_TYPE=" + body.GetContentType());
// 	}
// 	else
// 	{
// 		env.push_back("CONTENT_LENGTH=0");
// 		env.push_back("CONTENT_TYPE=");
// 	}
// 	env.push_back("PYTHONUNBUFFERED=1");
// 	_envp = new char*[env.size() + 1];
// 	for (size_t i = 0; i < env.size(); i++)
// 	{
// 		_envp[i] = new char[env[i].size() + 1];
// 		std::strcpy(_envp[i], env[i].c_str());
// 	}
// 	_envp[env.size()] = NULL;
// }


void ExecCGI::SetArgv(Location &location, std::string &path)
{
	std::vector<std::string> argv;
	if (_ext == ".php")
		argv.push_back(location.GetCGIPass(_ext));
	else
	{
		argv.push_back(location.GetCGIPass(_ext));
		argv.push_back(path);
	}
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

void ExecCGI::SetCgibody(std::string str)
{
	_cgibody = str;
}

void ExecCGI::SetTimeBeginCGI()
{
	time_t now = time(NULL);
	this->_time_begin_cgi = now;
}

void ExecCGI::DeleteArgvEnvp()
{
	if (_argv)
	{
		for (int i = 0; _argv[i]; i++)
			delete [] _argv[i];
		delete [] _argv;
	}
	if (_envp)
	{
		for (int i = 0; _envp[i]; i++)
			delete [] _envp[i];
		delete [] _envp;
	}
}

int ExecCGI::Execution(ParseRequest &header, ParseBody& body, SocketServer socket_server, Epoll& epoll)
{
	std::cout << "\n========== DEBUT EXECUTION CGI ==========" << std::endl;
	
	std::string path = SetupPath(header.GetPath(), _loc.GetName(), _loc.GetRoot());
	
	SetArgv(_loc, path);
	SetEnvp(header, body, path, socket_server);
	
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) == -1)
	{
		std::cerr << "[ERROR] pipe_in failed: " << strerror(errno) << std::endl;
		return (500);
	}
	if (pipe(pipe_out) == -1)
	{
		std::cerr << "[ERROR] pipe_out failed: " << strerror(errno) << std::endl;
		return (500);
	}

	epoll.SetEpoll(pipe_in[1], EPOLLOUT);
	epoll.SetEpoll(pipe_in[0], EPOLLIN);
	
	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "[ERROR] fork failed: " << strerror(errno) << std::endl;
		return (500);
	}
	
	if (pid == 0)
	{
		close(pipe_in[1]);
		close(pipe_out[0]);
		
		if (dup2(pipe_in[0], STDIN_FILENO) == -1)
		{
			std::cerr << "[CHILD ERROR] dup2 stdin: " << strerror(errno) << std::endl;
			exit(500);
		}
		if (dup2(pipe_out[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "[CHILD ERROR] dup2 stdout: " << strerror(errno) << std::endl;
			exit(500);
		}
		
		close(pipe_in[0]);
		close(pipe_out[1]);

		execve(_loc.GetCGIPass(_ext).c_str(), GetArgv(), GetEnvp());
		if (errno == ENOENT)
			exit(404);
		if (errno == EACCES)
			exit(403);
		else
			exit(500);
	}
	else
	{
		std::cout << "test8" << std::endl;
		SetTimeBeginCGI();
		close(pipe_in[0]);
		close(pipe_out[1]);
		_pid = pid;
		_fdin = pipe_in[1];
		_fdout = pipe_out[0];
		DeleteArgvEnvp();
	}
	return 0;
}

int ExecCGI::ReadWrite(ParseBody& body)
{
	std::string bodyContent = body.GetBody();
	std::cout << "test3" << std::endl;
	if (_wrote == false && !bodyContent.empty())
	{
		ssize_t written = write(_fdin, bodyContent.c_str(), bodyContent.length());
		if (written == -1)
			std::cerr << "[PARENT ERROR] write failed: " << strerror(errno) << std::endl;
		close(_fdin);
	}
	_wrote = true;
	
	char buffer[4096];
	ssize_t bytesRead;
	std::cout << "test" << std::endl;
	if ((bytesRead = read(_fdout, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[bytesRead] = '\0';
		std::cout << "test4" << std::endl;
		_cgibody += buffer;
	}
	if (bytesRead == -1)
		std::cerr << "[PARENT ERROR] read failed: " << strerror(errno) << std::endl;
	if (bytesRead == 0)
	{
		close(_fdout);
		int status;
		int	error_code;
		waitpid(_pid, &status, WNOHANG);
		if (WIFEXITED(status)) 
			error_code = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			int sig = WTERMSIG(status);
			switch(sig)
			{
				case SIGKILL:
					error_code = 504;  // Gateway Timeout
					break;
				case SIGTERM:  // arrêt du service ou kill
					error_code = 503;  // Service Unavailable
					break;
				case SIGSEGV:
					error_code = 500;
					break;
				default:
					error_code = 500;
			}
		}
		else
			error_code = 500;
		return (error_code);
	}
	std::cout << "test2" << std::endl;
	return -1;
}


bool ExecCGI::CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers) /*reference ?*/
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
			if (loc.CheckMethod(header.GetMethod()))
			{
				_ext = ext;
				_loc = loc;
				return true;
			}	
		}
	}
	return false;
}

void	ExecCGI::KillAndClose()
{
	kill(_pid, SIGKILL);
	waitpid(_pid, NULL, 0);
	close(_fdin);
	close(_fdout);
}
