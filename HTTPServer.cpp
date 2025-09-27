#include "HTTPServer.hpp"
#include "ParseBodyRequest.hpp"
#include <cstring>

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{

}

//READ REQUEST UNTIL END OF HEADERS
//GET SIZE OF CONTENT_LENGTH(FOR BODY REQUEST)
void HTTPServer::readHeaderRequest(int client_fd, ParseRequest& request)
{
	char	c;
	bool	first = false;
	std::string	line;
	while (1)
	{
		recv(client_fd, &c, 1, 0);
		line += c;
		std::cout << c;
		if (c == '\n')
		{
			std::cout << line << std::endl;
			line.erase(line.size() - 1);
			if (!line.empty() && line.at(line.size() - 1) == '\r')
				line.erase(line.size() - 1);
			if (first == false && request.DivideFirstLine(line) == 0)
				return;
			first = true;
			if (line.empty())
				return;
			request.DivideHeader(line);
			line.clear();
		}
	}
}

void HTTPServer::handleRequest(Epoll epoll, int i, size_t server_index) /*epoll en ref ?*/
{
	ParseRequest header;
	ParseBody	body;
	ExecCGI cgi;
	int client_fd = epoll.getEvent(i).data.fd;
	int body_len = 0;

	if (epoll.getEvent(i).events & EPOLLIN)	//RECEIVE DATAS
	{
		std::cout << "------------REQUEST------------" << std::endl;
		readHeaderRequest(client_fd, header);
		body_len = body.FindBodyLen(header);
		if (body_len != 0)
		{
			this->_body_buf = new char[body_len];
			int r = 0;
			while (1)
			{
				r += recv(client_fd, this->_body_buf + r, body_len -r, 0);
				if (r >= body_len)
					break;
			}
			std::cout << "Body_buf : " << this->_body_buf << std::endl;
			body.ChooseContent(this->_body_buf);
		}
		epoll.SetClientEpollout(i, this->_socket_client);/*erreur selon le petit chat*/
	}

	if (epoll.getEvent(i).events & EPOLLOUT)	//SEND DATAS
	{
		cgi.CheckCGI(header, body, servers);
		Response resp(this->servers[server_index].GetErrorPath(), client_fd, body_len);
		resp.sendResponse(header, this->_body_buf);
		close(client_fd);
		epoll.deleteClient(client_fd);
		body_len = 0;
	}
}

//PARSING CONF
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

//PARSING CONF
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

//PARSING CONF
std::vector<ServerConf> HTTPServer::ParsingConf(std::string conf_file)
{
	std::vector<ServerConf> servers;

	std::ifstream conf(conf_file.c_str());
	std::string line;
	while (std::getline(conf, line)) /*check doucl" {}, mauvais donné, fin ;*/
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
					temp.AddLocation(conf, line);
			}
			/* if (temp.GetErrorPath().empty())
			{
				temp.SetErrorPage(404, "<html><head><title>404 Not Found</title></head><body><center><h1>404 Not Found</h1></center><hr><center>MyWebServ</center></body></html>");
				temp.SetErrorPage(500, "<html><head><title>500 Internal Server Error</title></head><body><center><h1>500 Internal Server Error</h1></center><hr><center>MyWebServ</center></body></html>");
			} */
			servers.push_back(temp);
		}
	}
	return servers;
}

void HTTPServer::displayServers()
{
	size_t j = servers.size();
	for (size_t r = 0; r < j; r++)
	{
		std::cout << "Serveur numero : " << r+1 << std::endl;
		for (size_t i = 0; i < this->servers[r].GetServerName().size() ;i++)
			std::cout << this->servers[r].GetServerName()[i] << std::endl;
		std::cout << "HOST :" << this->servers[r].GetPort(0) << std::endl;/*pourquoi 0 ?*/
		std::cout << "PORT :" << this->servers[r].GetHost(0) << std::endl;/*pourquoi 0 ?*/
		std::cout << "ClientBody :" << this->servers[r].GetClientBodySize() << std::endl;
		std::cout << "Error 404 :" << this->servers[r].GetErrorPath(404) << std::endl;
		std::cout << "Error 500 :" << this->servers[r].GetErrorPath(500) << std::endl;
		for (int f = 0; f < this->servers[r]._nb_location; f++)
		{
			std::cout << "location numero : " << f+1 << std::endl;
			std::cout << "NAME Location :" << servers[r].GetLocation(f).GetName() << std::endl;
			std::cout << "ROOT Location :" << servers[r].GetLocation(f).GetRoot() << std::endl;
			for (int i = 0; i < servers[r].GetLocation(f).nb_methods; i++)
				std::cout << "ME Location :" << servers[r].GetLocation(f).GetMethods(i) << std::endl;
			std::cout << "AUTOINNDEX Location : " << servers[r].GetLocation(f).GetAutoindex() << std::endl;
			std::cout << "CGI Location : " << servers[r].GetLocation(f).GetCGIPass() << std::endl;
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

int HTTPServer::runServer()
{
	Epoll epoll(this->_socket_server);

	while(true)
	{
		size_t a;
		(void) a;
		int n = epoll.epollWait();
		for(int i = 0; i < n; i++)
		{
			bool event_is_server = false;
			for (size_t j = 0; j < this->servers.size(); j++)
			{
				a = j;
				if(epoll.getEvent(i).data.fd == this->_socket_server[j])
				{
					this->_socket_client = accept(this->_socket_server[j], NULL, NULL);
					if (this->_socket_client < 0)
					{
						std::cerr << "Failed to grab socket_client." << std::endl;
						return 1;
					}
					epoll.setClientEpollin(this->_socket_client);
					event_is_server = true;
					this->_attached_server = j;
					/*on peut break?*/
				}
			}
			if(!event_is_server)
			{
				handleRequest(epoll, i, this->_attached_server); /*set dans if precedent dc tjs obliger detre d abord serveur aant client ?*/
			}
			std::cout << std::endl;
		}
	}
	std::cout << "Loop exited" << std::endl;
	return 0;
}

int HTTPServer::startServer(std::string conf_file)
{
	this->servers = ParsingConf(conf_file);

	displayServers();

	if (prepareServerSockets() == 1)
		return 1;

	if (runServer() == 1)
		return 1;
	return 0;
}

void HTTPServer::closeServer()
{
	size_t size = this->_socket_server.size();
	for (size_t i = 0; i < size; i++)
		close(this->_socket_server[i]);
}

uint32_t HTTPServer::prepareAddrForHtonl(std::string addr)
{
	uint32_t ret = 0;
	std::vector<int> v;

	for(size_t i = 0; i < addr.size(); i++)
	{
		int octet = atoi(addr.c_str() + i);
		v.push_back(octet);
		while(addr[i] && addr[i] != '.')
			i++;
	}
	ret = (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3]; /*si  erreur et pas v3 ?*/
	v.clear();
	return ret;
}

bool HTTPServer::checkPortHostTaken(std::vector<std::pair<std::string, int> >host_port, std::string host, int port)
{
	for(size_t i = 0; i < host_port.size(); i++)
	{
		if (host == host_port[i].first && port == host_port[i].second)
		{
			return true;
		}
	}
	return false;
}

int HTTPServer::createServerSocket(std::vector<std::pair<std::string, int> > &host_port, size_t i, size_t j)
{
	std::string host = this->servers[i].GetHost(j);
	int port = this->servers[i].GetPort(j);
	bool host_port_taken = checkPortHostTaken(host_port, host, port);

	if(!host_port_taken)
	{
		//Linux kernel creates a new socket (point de communication)
		int socket_server = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_server < 0)
		{
			std::cerr << "Cannot create socket" << std::endl;
			return 1;
		}

		sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = htonl(prepareAddrForHtonl(host));
		sockaddr.sin_port = htons(port);

		host_port.push_back(std::make_pair(host, port));
		
		if (bind(socket_server, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		{
			std::cerr << "Failed to bind to port " << port << "." << std::endl;
			return 1;
		}

		if (listen(socket_server, 10))
		{
			std::cerr << "Failed to listen on socket." << std::endl;
			return 1;
		}
		this->_socket_server.push_back(socket_server);
	}
	return 0;
}

int HTTPServer::prepareServerSockets()
{
	std::vector<std::pair<std::string, int> > host_port;

	for (size_t i = 0; i < this->servers.size(); i++)
	{
		for(size_t j = 0; j < this->servers[i].GetHostPortSize(); j++)
		{
			if (createServerSocket(host_port, i, j) == 1)
				return 1;
		}
	}

	return 0;
}
