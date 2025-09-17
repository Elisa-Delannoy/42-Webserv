#include "HTTPServer.hpp"
#include "ParseRequest.hpp"
#include <cstring>  /*ELISA*/

HTTPServer::HTTPServer()
{
	//can't do that because we don't know the size yet
	// memset(this->_header_buf, 0, sizeof(this->_header_buf));  /*ELISA*/
	this->_size_header_buf = 0;
	this->_size_body_buf = 0;
}

HTTPServer::~HTTPServer()
{

}

// const char* HTTPServer::GetRequest(void) const
// {
// 	return (this->_header_buf);
// }


//READ REQUEST UNTIL END OF HEADERS
//GET SIZE OF CONTENT_LENGTH(FOR BODY REQUEST)
void HTTPServer::readHeaderRequest(int client_fd, std::string & header)
{
	char c;

	while (1)
	{
		recv(client_fd, &c, 1, 0);
		header += c;
		if (header.size() >= 4 && header.substr(header.size() - 4) == "\r\n\r\n")	//END OF HEADER
			break;
	}

	size_t pos = header.find("Content-Length:");
	if (pos != std::string::npos)
	{
		std::istringstream iss(header.substr(pos + 15));
		iss >> this->_size_body_buf;
	}
	this->_size_header_buf = header.size()+1;
}

//ADD HEADERS REQUEST IN CHAR*
void HTTPServer::getHeaderRequest(int client_fd)
{
	std::string header;
	readHeaderRequest(client_fd, header);

	this->_header_buf = new char[this->_size_header_buf+1];
	int i = 0;
	for(; i < this->_size_header_buf; i++)
	{
		this->_header_buf[i] = header[i];
	}
	this->_header_buf[i] = '\0';
}

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
				ParseRequest request(this->_header_buf);	//moved request here because it is needed to send the response (second if)
				int client_fd = epoll.getEvent(i).data.fd;

				if (epoll.getEvent(i).events & EPOLLIN)	//RECEIVE DATAS
				{
					std::cout << "------------REQUEST------------" << std::endl;

					getHeaderRequest(client_fd);

					// memset(this->_header_buf, 0, this->_size_buf);  /*ELISA*/

					//HERE RECV GOT THE HEADERS, WE CONTINUE TO SEE IF THERE IS A BODY
					//FOR EXAMPLE AN UPLOAD
					if (this->_size_body_buf != 0)
						recv(client_fd, this->_body_buf, this->_size_body_buf, 0);

					epoll.SetClientEpollout(i, this->_socket_client);
					std::cout << this->_header_buf << std::endl;
					request.DivideRequest();
				}

				if (epoll.getEvent(i).events & EPOLLOUT)	//SEND DATAS
				{
					Response resp(client_fd);
					resp.sendHeaders(request);
					resp.sendContent(request, this->_body_buf, this->_size_body_buf);
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
