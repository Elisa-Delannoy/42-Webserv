#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	_host = "";
	_port = 0;
	_client_body_size = 0;
}

ServerConf::~ServerConf()
{
}

void ServerConf::SetServerName(std::string server_name)
{
	_server_name.push_back(server_name);
}

void ServerConf::SetHost(std::string host)
{
	_host = host;
}

void ServerConf::SetPort(int port)
{
	_port = port;
}

void ServerConf::SetClientBodySize(int size)
{
	_client_body_size = size;
}

std::vector<std::string> ServerConf::GetServerName() const
{
	return _server_name;
}