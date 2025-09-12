#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	_server_name = "";
	_host = "";
	_port = 0;
	_client_body_size = 0;
}

ServerConf::~ServerConf()
{
}

void ServerConf::SetServerName(std::string server_name)
{
	_server_name = server_name;
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

