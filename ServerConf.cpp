#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	_client_body_size = 0;
}

ServerConf::~ServerConf()
{
}

void ServerConf::SetServerName(std::string server_name)
{
	_server_name.push_back(server_name);
}

void ServerConf::SetHostPort(std::string host, int port)
{
	_host_port.push_back(std::make_pair(host, port));
}

void ServerConf::SetClientBodySize(int size)
{
	_client_body_size = size;
}

void ServerConf::SetErrorPage(int type_error, std::string path)
{
	_error_pages[type_error] = path;
}

void ServerConf::SetStaticLocation(StaticLocation location)
{
	_static_location.push_back(location);
}

std::vector<std::string> ServerConf::GetServerName() const
{
	return _server_name;
}

int ServerConf::GetClientBodySize() const
{
	return _client_body_size;
}

int ServerConf::GetPort(int nb) const
{
	return _host_port[nb].second;
}

std::string ServerConf::GetHost(int nb) const
{
	return (_host_port[nb].first);
}

std::string ServerConf::GetErrorPath(int type_error)
{
	return (_error_pages[type_error]);
}


void ServerConf::AddServerName(ServerConf& server, std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "server_name")
		return ;
	while (ss >> word)
	{
		if (!word.empty() && word[(word.length() - 1)] == ';')
		{
			word.erase(word.length() - 1);
			server.SetServerName(word);
			break;
		}
		server.SetServerName(word);
	}
}

int CheckValue(std::string port)
{
	int intport = atoi(port.c_str());
	std::ostringstream oss;
	oss << intport;
	if (oss.str() != port)
		return -1;
	return intport;
}

void ServerConf::AddHostPort(ServerConf& server, std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "listen")
		return ;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	int sep = word.find(':');
	int port = CheckValue(word.substr(sep + 1, word.length()).c_str());
	if (port == -1)
		return;
	server.SetHostPort(word.substr(0, sep), port);
}

int ClientBodyValue(int value, int multiplier)
{
	switch (multiplier)
	{
	case 'K':
		value = value * 1000;
		break;
	case 'M':
		value = value * 1000000;
		break;
	case 'G':
		value = value * 1000000000;
		break;
	default:
		break;
	}
	return value;
}

void ServerConf::AddClientBody(ServerConf& server, std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "client_max_body_size")
		return ;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	int value = atoi(word.substr(0, word.length() - 1).c_str());
	char multiplier = word.substr(word.length() - 1).c_str()[0];
	server.SetClientBodySize(ClientBodyValue(value, multiplier));
}

void ServerConf::AddErrorPage(ServerConf& server, std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "error_page")
		return ;
	ss >> word;
	int type_error = CheckValue(word);
	if (type_error == -1)
		return;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	server.SetErrorPage(type_error, word);
}

void ServerConf::AddStaticLocation(ServerConf& server, std::ifstream& conf)
{
	(void)server;
	std::string line;
	std::getline(conf, line);
	std::cout << line << std::endl;
}
