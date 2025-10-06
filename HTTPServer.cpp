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

void	printvec(std::vector<char>::iterator begin, std::vector<char> vec)
{
	std::cout << "beginVEC\n";
	for (; begin != vec.end(); begin++)
	{
		std::cout << *(begin);
	}
	std::cout << "\n\n\n\n\nend\n\n\n" << std::endl;
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
bool HTTPServer::ParsingConf(std::string conf_file)
{
	std::ifstream conf(conf_file.c_str());
	std::string line;
	while (std::getline(conf, line)) /*check doucle {}, mauvaise donnée, fin ;*/
	{
		if (CheckServerStart(line) == true)
		{
			ServerConf temp;
			while (true)
			{	
				int error = 0;
				std::getline(conf, line);
				if (line.find("}") != std::string::npos)
					break;
				else if (temp.isComment(line) == true)
					continue;
				else if (line.find("server_name") != std::string::npos)
					error = temp.AddServerName(temp.removeInlineComment(line));
				else if (line.find("client_max_body_size") != std::string::npos)
					error = temp.AddClientBody(temp.removeInlineComment(line));
				else if (line.find("listen") != std::string::npos)
					error = temp.AddHostPort(temp.removeInlineComment(line));
				else if (line.find("error_page") != std::string::npos)
					error = temp.AddErrorPage(temp.removeInlineComment(line));
				else if (CheckLocationStart(line) == true)
					error = temp.AddLocation(conf, temp.removeInlineComment(line));
				else
					error = 9;
				if (error != 0)
					return (temp.Error(error), false);
			}
			this->servers.push_back(temp);
		}
	}
	return true;
}

void HTTPServer::displayServers()
{
	size_t j = servers.size();
	for (size_t r = 0; r < j; r++)
	{
		std::cout << "Serveur numero : " << r+1 << std::endl;
		for (size_t i = 0; i < this->servers[r].GetServerName().size() ;i++)
			std::cout << this->servers[r].GetServerName()[i] << std::endl;
		std::cout << "HOST :" << this->servers[r].GetPort(0) << std::endl;
		std::cout << "PORT :" << this->servers[r].GetHost(0) << std::endl;
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
			// std::cout << "CGI Location : " << servers[r].GetLocation(f).GetCGIPass() << std::endl;
			std::cout << "Index : " << servers[r].GetLocation(f).GetIndex() << std::endl;
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

int	HTTPServer::GetServerIndex(int used_socket)
{
	for (size_t j = 0; j < this->servers.size(); j++)
	{
		if(used_socket == this->_socket_server[j])
			return (j);
	}
	return (-1);
}

int HTTPServer::readHeaderRequest(int client_fd, Clients* client, std::vector<char> request)
{

	bool		first = false;
	std::string	line;
	int begin = 0;
	(void) client_fd;

	for (size_t i = 0; i < request.size(); i++)	
	{
		if (request[i] == '\n')
		{
			std::string	line(request.begin() + begin, request.begin() + i + 1);
			line.erase(line.size() - 1);
			if (!line.empty() && line.at(line.size() - 1) == '\r')
				line.erase(line.size() - 1);
			// std::cout << line << std::endl; /*A SUPP*/
			if (first == false && client->_head.DivideFirstLine(line) == 0)
				return (-1);
			first = true;
			if (line.empty())
				return (i);
			client->_head.DivideHeader(line);
			line.clear();
			begin = i + 1;
		}
	}
	return (0);
}

int	HTTPServer::CheckEndWithChunk(Clients* client, std::vector<char> buffer)
{
	(void) client;
	// if (!client->_body.GetChunk())
	// 	return (0);
	int	line = 1;
	int	i = 0;

	std::cout << "CHUNK = " << std::endl;
	std::cout << "buff 0 = " << buffer[0] << std::endl;

	for (std::vector<char>::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		std::cout << "DANS WHILE BUFFER" << std::endl;
		int begin = i;
		if (line % 2 != 0)
		{	
			for (; buffer[i] !='\n'; i++)
				continue;
			std::string size(buffer + begin, buffer + i + 1);
			std::cout << "size = " << size << std::endl;
			// int bytes = 
			line++;
		}
		return (0);
		// if (line)
	}
	
	return (1);
}

int	HTTPServer::CheckEndWithLen(Clients* client)
{
	int	len = client->_body.GetContentLen();
	std::cout << "LEN = " << len << std::endl;
	if (len == 0)
		return (0);
	if (client->GetReadBuffer().size() - client->_head.GetIndexEndHeader() >= static_cast<size_t>(len)) /*verifier si -1 a jouter ou autre*/
		return (1);
	return (0);
}

int	HTTPServer::CheckEndRead(Clients* client, char* buffer)
{
	const char* endheader = "\r\n\r\n";
	// std::cout << "dans check" << std::endl;
	if (client->GetReadHeader() == false)
	{
		// std::cout << "IN R_HEAD" << std::endl;
		std::vector<char>::iterator it = std::search(client->GetReadBuffer().begin(), client->GetReadBuffer().end(), 
			endheader, endheader + 4);
		if (it == client->GetReadBuffer().end())
			return (-1);
		int i = readHeaderRequest(client->GetSocket(), client, client->GetReadBuffer());
		if (i > 0)
		{
			// std::cout << "I >= 0" << std::endl;
			client->_head.SetIndexEndHeader(i);
			client->_body.SetPreviousSize(i + 1);
		}
		else
		{
			std::cout << "-1 NO BODY" << std::endl;
			return (-1);
		}
		// else if (i == 0)
		client->SetReadHeader(true);
	}
	if (!client->_body.IsBody(client->_head))
	{
		client->_head.SetIndexEndHeader(0);
		// client->SetReadHeader(false);
		return (1);
	}
	else
	{
		if (client->GetReadBuffer().begin() + client->_body.GetPreviousSize() < client->GetReadBuffer().end())
		{
			std::vector<char>	temp(client->GetReadBuffer().begin() + client->_body.GetPreviousSize() , client->GetReadBuffer().end());
			if (CheckEndWithChunk(client,  temp) == 1 || CheckEndWithLen(client) == 1)
				return (std::cout << "DANS IF 2" << std::endl, 1);
		}
	}
	client->_body.SetPreviousSize(client->GetReadBuffer().size());
	return (0);
}


void HTTPServer::ReadAllRequest(Clients* client, int fd)
{
	char	buffer[4096];
	int		bytes = recv(fd, buffer, sizeof(buffer), 0);

	std::cout << "lecture : " << bytes << std::endl;
	// while (bytes > 0)
	// {
		std::cout << "ENTER LOOP READ" << std::endl;
		client->SetReadBuff(buffer, bytes);
		for (int i= 0; i < bytes; i++)
			std::cout << buffer[i];
		std::cout << "\n fin buffer \n" << std::endl;

		if (CheckEndRead(client, buffer) > 0)
		{

			client->SetStatus(Clients::PARSING_REQUEST);
			// break;
		}
		if (bytes == 0)
		{
			client->SetStatus(Clients::CLOSED);
			// break;
		}
		if (bytes == -1)
		{
			client->SetStatus(Clients::CLOSED);
			// break;
		}
		//recv again because otherwise we're not reading the rest?
		//with this it works
		// bytes = recv(fd, buffer, sizeof(buffer), 0);
	// }
	// if (bytes == 0)
	// {
	// 	client->SetStatus(Clients::PARSING_REQUEST);
	// }
	// std::cout << "END LOOP READ" << std::endl;
	// std::cout << "bytes read : " << bytes << std::endl;
	// if (bytes == -1)
	// 	client->SetStatus(Clients::CLOSED);
		// errno impossible, considerer comme a essayer plus trd ou fermer le socket ? 
}

void HTTPServer::handleRequest(Epoll& epoll, int i, Clients* client)
{
	// client->_head;
	// ParseRequest	header;
	ParseBody		body;
	ExecCGI 		cgi;
	std::vector<char> request;

	int client_fd = epoll.getEvent(i).data.fd;
	int body_len = 0;

	if (epoll.getEvent(i).events & EPOLLIN && client->GetStatus() == Clients::WAITING_REQUEST)
	{
		std::cout << "------------REQUEST------------" << client_fd << std::endl;

		ReadAllRequest(client, client_fd);
	}
	if (client->GetStatus() == Clients::PARSING_REQUEST)
	{
		std::cout << "BEFORE READ BUFFER" << std::endl;
		request = client->GetReadBuffer();
		body_len = client->_body.GetContentLen();
		std::cout << body_len << std::endl;
		std::cout << "BEFORE CHOOSE CONTENT -- buffer size = " << request.size() << std::endl;
		if (client->_body.IsBody(client->_head))
		{
			request.erase(request.begin(), request.begin() + client->_head.GetIndexEndHeader() + 1);
			client->_body.ChooseContent(request);
			// std::cout << "\n\n--------BUF BEGIN--------" << std::endl;
			// std::vector<char>::iterator it = request.begin();
			// for(; it != request.end(); it++)
			// 	std::cout << *it;
			// std::cout << "\n--------BUF END-------\n" << std::endl;
		}
		client->ClearBuff();
		client->_head.SetIndexEndHeader(0);
		client->SetReadHeader(false);
		client->SetStatus(Clients::SENDING_RESPONSE);
		std::cout << "BEFORE EPOLLOUT" << std::endl;
	}

	if (epoll.getEvent(i).events & EPOLLOUT && client->GetStatus() == Clients::SENDING_RESPONSE)
	{
		std::string cgihtml = cgi.CheckCGI(client->_head, client->_body, servers[client->GetServerIndex()]);
		std::cout << cgihtml << std::endl;
		// cgi.CheckCGI(client->_head, client->_body, servers[client->GetServerIndex()]);
		Response resp(this->servers[client->GetServerIndex()], client);
		std::cout << "BEFORE RESPONSE" << std::endl;
		resp.sendResponse(this->servers[client->GetServerIndex()], client, request);
		// close(client_fd);
		client->SetStatus(Clients::WAITING_REQUEST);
		// epoll.deleteClient(client_fd);
		body_len = 0;
	}
}

int	HTTPServer::AcceptRequest(int used_socket, Epoll& epoll, int j)
{
	// std::cout << "accept " << used_socket << std::endl;
	(void) used_socket;
	int socket = accept(this->_socket_server[j], NULL, NULL);
	if (socket < 0)
	{
		std::cerr << "Failed to grab socket_client." << std::endl;
		return (-1);
	}
	Clients*	client = new Clients(socket, j); /*voir ou delete*/
	this->_socket_client[socket] = client;
	epoll.SetEpoll(socket);
	return (1);	
}

Clients*	HTTPServer::FindClient(int fd)
{
	std::map<int, Clients*>::iterator it = this->_socket_client.find(fd);
	if (it != this->_socket_client.end())
		return (it->second);
	else
		return NULL;
}

int HTTPServer::runServer()
{
	Epoll epoll(this->_socket_server);
	int	server_index;

	while(true)
	{
		int n = epoll.epollWait();
		for (int i = 0; i < n; i++)
		{
			int	used_socket = epoll.getEvent(i).data.fd;
			server_index = GetServerIndex(used_socket);
			if (server_index >= 0)
				AcceptRequest(used_socket, epoll, server_index); /*voir quoi faire si erreur*/
			else 
			{
				// Clients* client = this->_socket_client[used_socket];
				Clients* client = FindClient(used_socket);
				if (client != NULL)
					handleRequest(epoll, i, client);
			}
			std::cout << std::endl;
		}
	}
	std::cout << "Loop exited" << std::endl;
	return 0;
}

int HTTPServer::startServer(std::string conf_file)
{
	if (ParsingConf(conf_file) == false)
		return 1;

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
