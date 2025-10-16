#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <signal.h>
#include "ServerConf.hpp"
#include "Response.hpp"
// #include "ExecCGI.hpp"
#include "Epoll.hpp"
#include "ParseBody.hpp"
#include "SocketServer.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		int		startServer(std::string conf_file);
		int		runServer();
		void	closeServer();
		int		prepareServerSockets();
		int		createServerSocket(std::vector<std::pair<std::string, int> > &host_port, size_t i, size_t j);
		void	AcceptRequest(Epoll& epoll, int j);
		void	CleanClient(int client_fd, Epoll& epoll);

		int	GetServerIndex(int used_socket);

		//Parsing .conf
		bool ParsingConf(std::string conf_file);
		void displayServers();
		std::vector<ServerConf> servers;

		int		readHeaderRequest(Clients* client, std::vector<char> request);
		void	HandleAfterReading(std::vector<char>& request, Clients* client);
		void	handleRequest(Epoll& epoll, int i, Clients* client);
		bool	UniqueClient(std::map<int, Clients*>::iterator it);

		uint32_t 	prepareAddrForHtonl(std::string addr);
		bool 		checkPortHostTaken(std::vector<std::pair<std::string, int> >host_port, std::string host, int port);
		Clients*	FindClient(int fd, int & id);
		void 		ReadAllRequest(Clients* client, int fd);
		int			CheckEndRead(Clients* client);
		int			CheckEndWithChunk(Clients* client);
		int			CheckEndWithLen(Clients* client);
		void		HandleCGI(Epoll& epoll, Clients* client, int i);
		void		HandleExcevCGI(Epoll& epoll, Clients* client, int i);
		void		CleanCGI(int fd, Epoll& epoll);
		void		CleanForTimeout(Clients* client, Epoll& epoll);

	private:
		std::vector<SocketServer>		_socket_server;
		std::map<int, Clients*>	_socket_client;
		std::map<int, size_t>	_attached_server;
		int						_counter_id;
};

#endif