#pragma once

#include <string>

class SocketServer
{
	private :
		int _fd;
		int _port;
		std::string _host;
		int _server_index;
	public:
		SocketServer();
		SocketServer(int fd, int port, std::string host, int server_index);
		~SocketServer();

		int GetFd() const;
		int GetPort() const;
		std::string GetHost() const;
		int GetServerIndex() const;

		void Setfd(int& fd);
		void SetPort(int& port);
		void SetHost(std::string& host);
};