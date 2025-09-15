#include "HTTPServer.hpp"
#include "ParseRequest.hpp"
#include <cstring>  /*ELISA*/

HTTPServer::HTTPServer()
{
	memset(this->_buf, 0, sizeof(this->_buf));  /*ELISA*/
}

HTTPServer::~HTTPServer()
{

}

// const char* HTTPServer::GetRequest(void) const
// {
// 	return (this->_buf);
// }

/*
Imagine une boîte aux lettres :

Côté client : tu fabriques une boîte (socket),
tu vas directement poster ta lettre (connect)
→ la communication est possible.

Côté serveur : tu fabriques une boîte (socket),
tu dis “je la pose devant la maison au numéro 8080” (bind),
et tu annonces “je suis prêt à recevoir du courrier” (listen)
→ tu attends que quelqu’un vienne.
*/



int HTTPServer::startServer()
{
	if (prepareServerSocket() == 1)
		return 1;

	//----------------CLIENT SOCKET----------------------

	Epoll epoll(this->_socket_server);

	while(true)
	{
		int n = epoll.epollWait();
		for(int i = 0; i < n; i++)
		{
			if(epoll.getEvent(i).data.fd == this->_socket_server)
			{
				this->_socket_client = accept(this->_socket_server, NULL, NULL);
				if (this->_socket_client < 0)
				{
					std::cerr << "Failed to grab socket_client." << std::endl;
					return 1;
				}
				epoll.setClientEpollin(this->_socket_client);
			}
			else
			{
				ParseRequest request(this->_buf);
				int client_fd = epoll.getEvent(i).data.fd;
				if (epoll.getEvent(i).events & EPOLLIN)
				{
					std::cout << "EPOLLIN" << std::endl;
					memset(this->_buf, 0, sizeof(this->_buf));  /*ELISA*/
					int r = recv(client_fd, this->_buf, sizeof(this->_buf), 0);
					if (r <= 0)
						close(client_fd);

					std::cout << this->_buf << std::endl;

					epoll.SetClientEpollout(i, this->_socket_client);
					request.DivideRequest();
				}
				if (epoll.getEvent(i).events & EPOLLOUT)
				{
					Response resp(client_fd);
					if (request.GetPath() == "/")
					{
						resp.displayBody();
					}
					else
					{
						resp.displayImg();
					}
					close(client_fd);
					epoll.deleteClient(client_fd);
				}
			}
		}
	}	
	return 0;
}

void HTTPServer::closeServer()
{
	// close(socket_client);a
	close(this->_socket_server);
}

int HTTPServer::prepareServerSocket()
{
	//----------------SERVER SOCKET----------------------

	//Linux kernel creates a new socket (point de communication)
	this->_socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_server < 0)
	{
		std::cerr << "Cannot create socket" << std::endl;
		return 1;
	}

	this->_sockaddr.sin_family = AF_INET;
	this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->_sockaddr.sin_port = htons(8080);
	//until here the socket exists, but isn't attached to any ports or IP address

	//bind socket on port 8080 (htons(8080))
	//INADDR_ANY => accept connections from any network interfaces (localhost, local IP, ...)
	//without bind, the socket doesn't know "where to live"
	if (bind(this->_socket_server, (struct sockaddr*)&this->_sockaddr, sizeof(this->_sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port 8080." << std::endl;
		return 1;
	}

	//until here, socket can communicate but cannot receive connections
	//with listen => transforms socket in a server socket.
	//add in queue up to 10 connections if accept is not done yet
	//listen does not read datas, it prepares the socket only
	if (listen(this->_socket_server, 10))
	{
		std::cerr << "Failed to listen on socket." << std::endl;
		return 1;
	}
	//socket becomes a server entry point
	return 0;
}
