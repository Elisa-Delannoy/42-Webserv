#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>

bool CheckServerStart(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "server")
		return false;
	ss >> word;
	if (word != "{")
		return false;
	ss >> word;
	if (word != "{")
		return false;
	return true;
}

bool CheckLocationStart(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "location")
		return false;
	while (word != "{")
	{
		std::string temp;
		temp = word;
		ss >> word;
		if (temp == word)
			break;
	}
	if (word != "{")
		return false;
	return true;
}

std::vector<ServerConf> ParsingConf()
{
	std::vector<ServerConf> servers;

	std::ifstream conf("conf/valid.conf");
	std::string line;
	while (std::getline(conf, line))
	{
		if (CheckServerStart(line) == true)
		{
			ServerConf temp;
			while (true)
			{	
				std::getline(conf, line);
				if (line.find("}") != std::string::npos)
					break;
				if (line.find("server_name") != std::string::npos)
					temp.AddServerName(temp, line);
				if (line.find("client_max_body_size") != std::string::npos)
					temp.AddClientBody(temp, line);
				if (line.find("listen") != std::string::npos)
					temp.AddHostPort(temp, line);
				if (line.find("error_page") != std::string::npos)
					temp.AddErrorPage(temp, line);
				if (CheckLocationStart(line) == true)
					temp.AddStaticLocation(temp, conf);
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
	std::cout << "Error 404 :" << servers[0].GetErrorPath(404) << std::endl;
}