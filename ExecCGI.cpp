#include "ExecCGI.hpp"

ExecCGI::ExecCGI() : _wrote(false), _read(false), _w_len(0), _count_read(0), _count_write(0), _time_begin_cgi(0)
{
}

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

int		ExecCGI::GetFdOut()
{
	return (this->_fdout);
}

int		ExecCGI::GetFdIn()
{
	return (this->_fdin);
}

bool	ExecCGI::GetRead()
{
	return (this->_read);
}

bool	ExecCGI::GetWrote()
{
	return (this->_wrote);
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

void ExecCGI::SetRead(bool state)
{
	this->_read = state;
}

void ExecCGI::SetWrote(bool state)
{
	this->_wrote = state;
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

void	ExecCGI::ChildExec(int pipe_in[2], int pipe_out[2])
{
	close(pipe_in[1]);
	close(pipe_out[0]);
	
	if (dup2(pipe_in[0], STDIN_FILENO) == -1 || dup2(pipe_out[1], STDOUT_FILENO) == -1)
	{
		std::cerr << "Error : dup2" << strerror(errno) << std::endl;
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

int ExecCGI::Execution(ParseRequest &header, ParseBody& body, SocketServer socket_server, Epoll& epoll)
{
	std::string path = SetupPath(header.GetPath(), _loc.GetName(), _loc.GetRoot());
	SetArgv(_loc, path);
	SetEnvp(header, body, path, socket_server);
	this->_count_read = 0;
	this->_count_write = 0;
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
		return (std::cerr << "Error: pipe failed: " << strerror(errno) << std::endl, 500);
	if (epoll.SetEpoll(pipe_in[1], EPOLLOUT) == 0 || epoll.SetEpoll(pipe_out[0], EPOLLIN) == 0)
		return (std::cerr << "Error: socket is not created\n" << std::endl, 500);
	pid_t pid = fork();
	if (pid == -1)
		return (std::cerr << "Error: fork failed: " << strerror(errno) << std::endl, 500);
	if (pid == 0)
		ChildExec(pipe_in, pipe_out);
	else
	{
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

int	ExitCode(int status)
{
	int error_code;

	if (WIFEXITED(status)) 
		error_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		int sig = WTERMSIG(status);
		switch(sig)
		{
			case SIGKILL:
				error_code = 504;
				break;
			case SIGTERM:  
				error_code = 503;
				break;
			default:
				error_code = 500;
		}
	}
	else
		error_code = 500;
	return (error_code);
}

int ExecCGI::Read(Epoll& epoll)
{
	char buffer[4096];
	ssize_t bytesRead = read(_fdout, buffer, sizeof(buffer) - 1);
	if (bytesRead  > 0)
	{
		buffer[bytesRead] = '\0';
		_cgibody += buffer;
	}
	else if (bytesRead == -1)
	{
		if (this->_count_read > 10)
			return (500);
		this->_count_read++;
	}
	else if (bytesRead == 0)
	{
		int status;
		if (waitpid(_pid, &status, WNOHANG) == 0)
			return (-1);
		return (ExitCode(status));
	}
	(void) epoll;
	return -1;
}

int ExecCGI::Write(ParseBody& body)
{
	std::string bodyContent = body.GetBody();
	if (!bodyContent.empty())
	{
		ssize_t written = write(_fdin, bodyContent.c_str() + this->_w_len, bodyContent.length() - this->_w_len);
		if (written == -1)
		{
			if (this->_count_write > 10)
				return (500);
			this->_count_write++;
		}
		else
		{
			this->_w_len += written;
			if (this->_w_len >= bodyContent.size())
			{
				this->_wrote = true;
				return (0);
			}
		}
		return (-1);
	}
	else
		this->_wrote = true;
	return (0);
}

bool ExecCGI::CheckCGI(ParseRequest &header, ParseBody &body, ServerConf &servers)
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

int	ExecCGI::Kill()
{
	if (this->_pid > 0)
	{
		kill(_pid, SIGKILL);
		if (waitpid(_pid, NULL, 0) != 0)
			return (1);	
	}
	return (0);
}
