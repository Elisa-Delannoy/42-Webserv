#include "HTTPServer.hpp"

HTTPServer::HTTPServer() : buffer_request("default")
{
	
}

HTTPServer::~HTTPServer()
{

}

const char* HTTPServer::GetRequest(void) const
{
	return (this->buffer_request);
}

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

	//leave server open for many clients (with while)
	// while(true)
	// {
	//accept : new socket for a client
	this->len = sizeof(this->sockaddr);
	this->socket_client = accept(this->socket_server, NULL, NULL);
	if (this->socket_client < 0)
	{
		std::cerr << "Failed to grab socket_client." << std::endl;
		return 1;
	}
	//read client request (HTTP request)
	// char buffer_request[1024];
	int bytes_received = recv(this->socket_client, buffer_request, sizeof(buffer_request) - 1, 0);
	if (bytes_received < 0)
	{
		std::cout << "Error while reading the client request" << std::endl;
		return 1;
	}
	// std::cout << "request\n" << this->buffer_request << std::endl;
	
	std::ifstream file("index.html");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	
	std::stringstream size;
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
	if(send(this->socket_client, response.c_str(), response.size(), 0) == -1)
	{
		std::cerr << "Error while sending." << std::endl;
		return 1;
	}
	close(this->socket_client);
	// }
	
	return 0;
}

void HTTPServer::closeServer()
{
	// close(socket_client);
	close(socket_server);
}

int HTTPServer::prepareServerSocket()
{
	//----------------SERVER SOCKET----------------------

	//Linux kernel creates a new socket (point de communication)
	this->socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (this->socket_server < 0)
	{
		std::cerr << "Cannot create socket" << std::endl;
		return 1;
	}

	this->sockaddr.sin_family = AF_INET;
	this->sockaddr.sin_addr.s_addr = INADDR_ANY;
	this->sockaddr.sin_port = htons(8080);
	//until here the socket exists, but isn't attached to any ports or IP address

	//bind socket on port 8080 (htons(8080))
	//INADDR_ANY => accept connections from any network interfaces (localhost, local IP, ...)
	//without bind, the socket doesn't know "where to live"
	if (bind(this->socket_server, (struct sockaddr*)&this->sockaddr, sizeof(this->sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port 8080." << std::endl;
		return 1;
	}

	//until here, socket can communicate but cannot receive connections
	//with listen => transforms socket in a server socket.
	//add in queue up to 10 connections if accept is not done yet
	//listen does not read datas, it prepares the socket only
	if (listen(this->socket_server, 10))
	{
		std::cerr << "Failed to listen on socket." << std::endl;
		return 1;
	}
	//socket becomes a server entry point
	return 0;
}
