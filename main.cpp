#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

void AddServerName(ServerConf& server, std::string line)
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

void AddHostPort(ServerConf& server, std::string line)
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

void AddClientBody(ServerConf& server, std::string line)
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

void AddErrorPage(ServerConf& server, std::string line)
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

std::vector<ServerConf> ParsingConf()
{
	std::vector<ServerConf> servers;

	std::ifstream conf("conf/valid.conf");
	std::string line;
	while (std::getline(conf, line))
	{
		if (line.find("server {") != std::string::npos)
		{
			ServerConf temp;
			while (true)
			{	
				std::getline(conf, line);
				if (line.find("}") != std::string::npos)
					break;
				if (line.find("server_name") != std::string::npos)
					AddServerName(temp, line);
				if (line.find("client_max_body_size") != std::string::npos)
					AddClientBody(temp, line);
				if (line.find("listen") != std::string::npos)
					AddHostPort(temp, line);
				if (line.find("error_page") != std::string::npos)
					AddErrorPage(temp, line);
			}
			servers.push_back(temp);
		}
	}
	return servers;
}

int main()
{
	std::vector<ServerConf> servers;

	servers = ParsingConf();
	for (size_t i = 0; i < servers[0].GetServerName().size() ;i++)
		std::cout << servers[0].GetServerName()[i] << std::endl;
	std::cout << "HOST :" << servers[0].GetPort(0) << std::endl;
	std::cout << "PORT :" << servers[0].GetHost(0) << std::endl;
	std::cout << "ClientBody :" << servers[0].GetClientBodySize() << std::endl;
	std::cout << "Error 404 :" << servers[0].GetErrorPath(403) << std::endl;
}