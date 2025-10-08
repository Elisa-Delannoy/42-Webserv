#include "HTTPServer.hpp"
#include "ParseBody.hpp"
#include <cstring>
#include <signal.h>

volatile sig_atomic_t g_running = 1;

HTTPServer::HTTPServer()
{
}

HTTPServer::~HTTPServer()
{
	std::map<int, Clients*>::iterator it = this->_socket_client.begin();
	for (; it != this->_socket_client.end(); it++)
	{
		close(it->first);
		delete it->second;
	}
	for(size_t i = 0; i < this->_socket_server.size(); i++)
		close(this->_socket_server[i]);
}

void handle_sigint(int sig)
{
	(void)sig;
	g_running = 0;
}


//READ REQUEST UNTIL END OF HEADERS
//GET SIZE OF CONTENT_LENGTH(FOR BODY REQUEST)

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
	while (std::getline(conf, line))
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

int	HTTPServer::CheckEndWithChunk(Clients* client)
{
	if (!client->_body.GetChunk())
		return (0);
	const char endchunk[] = { '\0', '\r', '\n' };
	const char endbody[] = { '\r', '\n' };

	std::vector<char>::iterator it = std::search(client->GetReadBuffer().begin(), client->GetReadBuffer().end(), 
		endchunk, endchunk + 3);
	
	// std::vector<char>::iterator temp;
	// temp = it;
	// // std::cout << "\n\n--------BUF BEGIN--------" << std::endl;
	// // std::vector<char>::iterator it = request.begin();
	// // for(; it != request.end(); it++)
	// // 	std::cout << *it;
	// // std::cout << "\n--------BUF END-------\n" << std::endl;
	if (it != client->GetReadBuffer().end())
	{
		std::vector<char>::iterator it_end = std::search(it + 2, client->GetReadBuffer().end(), 
		endbody, endbody + 2);

		if (it_end != client->GetReadBuffer().end())
		{
			std::cout << "retur 1" << std::endl;
			return (1);
		}
		std::cout << "3333" << std::endl;
		return (0);
	}
	std::cout << "44444" << std::endl;
	return (0);
}

int	HTTPServer::CheckEndWithLen(Clients* client)
{
	int	len = client->_body.GetContentLen();
	std::cout << "LEN = " << len << std::endl;
	if (len == 0)
		return (0);
	if (client->GetReadBuffer().size() - client->_head.GetIndexEndHeader() >= static_cast<size_t>(len)) /*to do verifier si -1 a jouter ou autre*/
		return (1);
	return (0);
}

int	HTTPServer::CheckEndRead(Clients* client)
{
	const char* endheader = "\r\n\r\n";
	if (client->GetReadHeader() == false)
	{
		std::vector<char>::iterator it = std::search(client->GetReadBuffer().begin(), client->GetReadBuffer().end(), 
			endheader, endheader + 4);
		if (it == client->GetReadBuffer().end())
			return (-1);
		int i = readHeaderRequest(client->GetSocket(), client, client->GetReadBuffer());
		if (i > 0)
			client->_head.SetIndexEndHeader(i);
		else if (i == 0)
			return (0);
		else
			return (-1);
		client->SetReadHeader(true);
	}
	if (!client->_body.IsBody(client->_head))
		return (1);
	else
	{
		if (CheckEndWithChunk(client) == 1 || CheckEndWithLen(client) == 1)
			return (1);
	}
	return (0);
}


void HTTPServer::ReadAllRequest(Clients* client, int fd)
{
	char	buffer[4096];
	int		bytes = recv(fd, buffer, sizeof(buffer), 0);

	std::cout << "lecture : " << bytes << std::endl;
	client->SetReadBuff(buffer, bytes);
	if (bytes > 0 && CheckEndRead(client) > 0)
		client->SetStatus(Clients::PARSING_REQUEST);
	if (bytes == 0)
		client->SetStatus(Clients::CLOSED);
	if (bytes == -1)
		client->SetStatus(Clients::CLOSED); // to do se rensiegner sur les flags et voir quoi faire avec -1
}

void HTTPServer::handleRequest(Epoll& epoll, int i, Clients* client)
{
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
		request = client->GetReadBuffer();
		if (request.empty())
		{
			client->SetStatus(Clients::WAITING_REQUEST);
			return;
		}
		body_len = client->_body.GetContentLen();
		if (client->_body.IsBody(client->_head))
		{
			std::cout << "IN IS BODY " << std::endl;
			request.erase(request.begin(), request.begin() + client->_head.GetIndexEndHeader() + 1);
			if (client->_body.GetChunk() == true)
				client->_body.ParseChunk(request);
			else
				client->_body.SetBody(request);
			client->_body.ChooseContent(request);
		}
		client->ClearBuff();
		client->_head.SetIndexEndHeader(0); // to do fonction clean client pour rmettre a 0 mais pas delete 
		client->SetReadHeader(false);
		client->SetStatus(Clients::SENDING_RESPONSE);
	}

	if (epoll.getEvent(i).events & EPOLLOUT && client->GetStatus() == Clients::SENDING_RESPONSE)
	{
		Response resp(this->servers[client->GetServerIndex()], client);
		if (!request.empty())
		{
			if (resp.sendResponse(this->servers[client->GetServerIndex()], client, request) == 0)
				client->SetStatus(Clients::CLOSED);;
				// to do FAIRE FONCITON CLOSE TOUT 
		}
		// close(client_fd);
		client->SetStatus(Clients::WAITING_REQUEST);
		// epoll.deleteClient(client_fd);
		body_len = 0;
	}
}

int	HTTPServer::AcceptRequest(Epoll& epoll, int j)
{
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

	signal(SIGINT, handle_sigint);

	while(g_running)
	{
		int n = epoll.epollWait();
		for (int i = 0; i < n; i++)
		{
			int	used_socket = epoll.getEvent(i).data.fd;
			server_index = GetServerIndex(used_socket);
			if (server_index >= 0)
				AcceptRequest(epoll, server_index);
			else 
			{
				Clients* client = FindClient(used_socket);
				if (client != NULL)
					handleRequest(epoll, i, client);
			}
		}
	}
	//to do gerer ctrl+C pour quitter proprement
	//=>fermer tous les sockets, etc
	return 0;
}

int HTTPServer::startServer(std::string conf_file)
{
	if (ParsingConf(conf_file) == false)
		return 1;

	displayServers(); // temporaire

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
		int socket_server = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_server < 0)
		{
			std::cerr << "Cannot create socket" << std::endl;
			close(socket_server);
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
			close(socket_server);
			return 1;
		}

		if (listen(socket_server, 10))
		{
			std::cerr << "Failed to listen on socket." << std::endl;
			close(socket_server);
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
