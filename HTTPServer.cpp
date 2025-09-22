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

bool CheckServerStart(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "server")
		return false;
	ss >> word;
	if (word != "{")
		return false;
	ss >> word;
	if (word != "{")
		return false;
	return true;
}

bool CheckLocationStart(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "location")
		return false;
	while (word != "{")
	{
		std::string temp;
		temp = word;
		ss >> word;
		if (temp == word)
			break;
	}
	if (word != "{")
		return false;
	return true;
}

std::vector<ServerConf> HTTPServer::ParsingConf()
{
	std::vector<ServerConf> servers;

	std::ifstream conf("conf/valid.conf");
	std::string line;
	while (std::getline(conf, line))
	{
		if (CheckServerStart(line) == true)
		{
			ServerConf temp;
			while (true)
			{	
				std::getline(conf, line);
				if (line.find("}") != std::string::npos)
					break;
				if (line.find("server_name") != std::string::npos)
					temp.AddServerName(line);
				if (line.find("client_max_body_size") != std::string::npos)
					temp.AddClientBody(line);
				if (line.find("listen") != std::string::npos)
					temp.AddHostPort(line);
				if (line.find("error_page") != std::string::npos)
					temp.AddErrorPage(line);
				if (CheckLocationStart(line) == true)
					temp.AddLocation(conf);
			}
			if (temp.GetErrorPath().empty())
				temp.SetErrorPage(404, "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>MyWebServ</center></body></html>");
			servers.push_back(temp);
		}
	}
	return servers;
}

int HTTPServer::startServer()
{
	std::vector<ServerConf> servers = ParsingConf();

	size_t j = servers.size();
	for (size_t r = 0; r < j; r++)
	{
		std::cout << "Serveru numero : " << r+1 << std::endl;
		for (size_t i = 0; i < servers[r].GetServerName().size() ;i++)
			std::cout << servers[r].GetServerName()[i] << std::endl;
		std::cout << "HOST :" << servers[r].GetPort(0) << std::endl;
		std::cout << "PORT :" << servers[r].GetHost(0) << std::endl;
		std::cout << "ClientBody :" << servers[r].GetClientBodySize() << std::endl;
		std::cout << "Error 404 :" << servers[r].GetErrorPath(404) << std::endl;
		for (int f = 0; f < servers[r]._nb_location; f++)
		{
			std::cout << "location numero : " << f+1 << std::endl;
			std::cout << "ROOT Location :" << servers[r].GetLocation(f).GetRoot() << std::endl;
			for (int i = 0; i < servers[r].GetLocation(f).nb_methods; i++)
				std::cout << "ME Location :" << servers[r].GetLocation(f).GetMethods(i) << std::endl;
			std::cout << "AUTOINNDEX Location : " << servers[r].GetLocation(f).GetAutoindex() << std::endl;
			std::cout << "CGI Location : " << servers[r].GetLocation(f).GetCGIPass() << std::endl;
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	
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
				}
				if (epoll.getEvent(i).events & EPOLLOUT)
				{
					std::cout << "EPOLLOUT" << std::endl;
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
