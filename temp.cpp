#include "HTTPServer.hpp"
#include "ServerConf.hpp"
#include <fstream>
#include <sstream>

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
}