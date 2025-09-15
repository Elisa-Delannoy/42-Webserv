#include "HTTPServer.hpp"
#include "ParseRequest.hpp"

HTTPServer::HTTPServer() : _buf("default")
{
	
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

void answerImg(int client_fd)
{
	struct stat img_info;

	if (stat("cookie.jpeg", &img_info) < 0)
	{
		std::cerr << "Error stat file" << std::endl;
		return;
	}
	std::ostringstream oss;
	oss << img_info.st_size;
	std::string size = oss.str();
	std::string response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: image/jpeg\r\n"
	"Content-Length: " + size + "\r\n"
	"Connection: keep alive\r\n"
	"\r\n";

	//SEND HEADERS
	if(send(client_fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Error while sending." << std::endl;

	//SEND BINARY DATAS
	std::ifstream ifs("cookie.jpeg", std::ios::binary);
	char *buffer = new char[img_info.st_size];
	ifs.read(buffer, img_info.st_size);
	int data_sent = 0;
	while(data_sent < img_info.st_size)
	{
		int data_read = send(client_fd, buffer + data_sent, img_info.st_size, 0);
		if(data_read == -1)
			std::cerr << "Error while sending." << std::endl;
		data_sent += data_read;
	}
	delete[] buffer;
}

int HTTPServer::startServer()
{
	if (prepareServerSocket() == 1)
		return 1;

	//----------------CLIENT SOCKET----------------------

	Epoll epoll(this->_socket_server);
	std::string path;

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
				int client_fd = epoll.getEvent(i).data.fd;
				if (epoll.getEvent(i).events & EPOLLIN)
				{
					std::cout << "EPOLLIN" << std::endl;

					int r = recv(client_fd, this->_buf, sizeof(this->_buf), 0);
					if (r <= 0)
						close(client_fd);

					std::cout << this->_buf << std::endl;

					epoll.SetClientEpollout(i, this->_socket_client);
					ParseRequest request(this->_buf);
					request.DivideRequest();
					path = request._path;
				}
				if (epoll.getEvent(i).events & EPOLLOUT)
				{
					if (path == "/")
					{
						std::ifstream file("index.html");
						std::stringstream buffer;
						std::stringstream size;

						buffer << file.rdbuf();
						std::string content = buffer.str();

						size << content.size();
						std::string content_size = size.str();

						//prepare response
						std::string response =
						"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: " + content_size + "\r\n"
						"Connection: close\r\n"
						"\r\n" + content;

						//send response
						if(send(client_fd, response.c_str(), response.size(), 0) == -1)
							std::cerr << "Error while sending." << std::endl;
					}
					else
					{
						answerImg(client_fd);
						
					}

					close(client_fd);
					epoll.deleteClient(client_fd);
					// std::cout << response << std::endl;
				}
			}
		}
	}	
	return 0;
}

void HTTPServer::closeServer()
{
	// close(socket_client);
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
