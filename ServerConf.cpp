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