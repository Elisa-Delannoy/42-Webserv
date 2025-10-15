#include "SocketServer.hpp"

SocketServer::SocketServer()
{
}

SocketServer::SocketServer(int fd, int port, std::string host, int server_index) : _fd(fd), _port(port), _host(host), _server_index(server_index)
{

}

SocketServer::~SocketServer()
{
}

int SocketServer::GetFd() const
{
	return _fd;
}

int SocketServer::GetPort() const
{
	return _port;
}

std::string SocketServer::GetHost() const
{
	return _host;
}

int SocketServer::GetServerIndex() const
{
	return _server_index;
}

void SocketServer::Setfd(int& fd)
{
	_fd = fd;
}

void SocketServer::SetPort(int& port)
{
	_port = port;
}

void SocketServer::SetHost(std::string& host)
{
	_host = host;
}
