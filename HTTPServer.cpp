#include "HTTPServer.hpp"

volatile sig_atomic_t g_running = 1;


HTTPServer::HTTPServer()
{
	this->_counter_id = 1;
}

HTTPServer::~HTTPServer()
{
	std::map<int, Clients*>::iterator it = this->_socket_client.begin();
	for (; it != this->_socket_client.end(); it++)
	{
		std::map<int, Clients*>::iterator same_it = it;
		close(it->first);
		if (UniqueClient(it, same_it) == true)
			delete it->second;
	}
	for(size_t i = 0; i < this->_socket_server.size(); i++)
		close(this->_socket_server[i].GetFd());
}

bool	HTTPServer::UniqueClient(std::map<int, Clients*>::iterator it, std::map<int, Clients*>::iterator same_it)
{
	same_it++;
	for (; same_it != this->_socket_client.end(); same_it++)
	{
		if (same_it->second == it->second)
			return (false);
	}
	return (true);
}

void handle_sigint(int sig)
{
	(void)sig;
	g_running = 0;
}

bool	Timeout(int last_time, int limit)
{
	time_t	now = time(NULL);

	if (now - last_time > limit)
		return (true);
	return (false);
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
	for (size_t j = 0; j < this->_socket_server.size(); j++)
	{
		if(used_socket == this->_socket_server[j].GetFd())
			return (j);
	}
	return (-1);
}

// -1 error
// 0 pas fini 
// i ok fin header
int HTTPServer::readHeaderRequest(Clients* client, std::vector<char> request)
{

	bool	first = false;
	std::string	line;
	int	begin = 0;

	for (size_t i = 0; i < request.size(); i++)	
	{
		if (request[i] == '\n')
		{
			std::string	line(request.begin() + begin, request.begin() + i + 1);
			line.erase(line.size() - 1);
			if (!line.empty() && line.at(line.size() - 1) == '\r')
				line.erase(line.size() - 1);
			if (first == false && client->_head.DivideFirstLine(line) == 0)
				return (client->SetStatus(Clients::SENDING_RESPONSE), -1);
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

int	HTTPServer::CheckEndWithChunk(Clients* client) /*to do voir si on fait une limite du nombre de chunk*/
{
	if (!client->_body.GetChunk())
		return (0);
	const char endchunk[] = { '0', '\r', '\n' };
	const char endbody[] = { '\r', '\n' };

	std::vector<char>::iterator it = std::search(client->GetReadBuffer().begin() + client->_head.GetIndexEndHeader(),
		client->GetReadBuffer().end(), endchunk, endchunk + 3);
	if (it != client->GetReadBuffer().end())
	{
		std::vector<char>::iterator it_end = std::search(it, client->GetReadBuffer().end(), 
		endbody, endbody + 2);
		if (it_end != client->GetReadBuffer().end())
			return (1);
		return (0);
	}
	return (0);
}

int	HTTPServer::CheckEndWithLen(Clients* client)
{
	int		len = client->_body.GetContentLen();
	size_t	read_body_len = client->GetReadBuffer().size() - client->_head.GetIndexEndHeader() - 1;

	if (len == 0)
		return (0);
	if (read_body_len == static_cast<size_t>(len))
		return (1);
	else if (read_body_len > static_cast<size_t>(len))
	{
		client->_head.SetForError(true, 404);
		client->SetStatus(Clients::SENDING_RESPONSE);
		return (-1);
	}
	else
		return (0);
}

// -1 error to send parse request
// 1 ok fini 
// 0 pas fini
int	HTTPServer::CheckEndRead(Clients* client)
{
	const char* endheader = "\r\n\r\n";;

	client->SetRecv(0);
	client->SetLastActivity();
	if (client->GetReadHeader() == false)
	{
		std::vector<char>::iterator it = std::search(client->GetReadBuffer().begin(), client->GetReadBuffer().end(), 
			endheader, endheader + 4);
		if (it == client->GetReadBuffer().end())
			return (0);
		int i = readHeaderRequest(client, client->GetReadBuffer());
		if (i > 0)
			client->_head.SetIndexEndHeader(i);
		else if (i == 0)
			return (-1);
		else
			return (0);
		client->SetReadHeader(true);
	}
	if (!client->_body.IsBody(client->_head))
		return (1);
	else if (CheckEndWithChunk(client) == 1 || CheckEndWithLen(client) == 1)
		return (1);
	if (client->_head.GetError() != 0)
		return (client->SetStatus(Clients::SENDING_RESPONSE), -1);
	return (0);
}

void HTTPServer::ReadAllRequest(Clients* client, int fd)
{
	char	buffer[4096];
	ssize_t		bytes = recv(fd, buffer, sizeof(buffer), 0);

	if (client->GetReadBuffer().size() == static_cast<size_t>(0))
		client->SetBeginRequest();
	if (Timeout(client->GetBeginRequest(), 60) == true)
	{
		client->_head.SetForError(true, 408);
		client->SetStatus(Clients::SENDING_RESPONSE);
		return ;
	}
	client->SetReadBuff(buffer, bytes);
	// if (client->GetReadBuffer().size() - client->GetServerIndex() > static_cast<size_t>(this->servers[client->GetServerIndex()].GetClientBodySize()))
	if (client->GetReadBuffer().size() > static_cast<size_t>(this->servers[client->GetServerIndex()].GetClientBodySize()))
		return (client->_head.SetForError(true, 413), client->SetStatus(Clients::SENDING_RESPONSE));
	if (bytes > 0 && CheckEndRead(client) > 0)
		client->SetStatus(Clients::PARSING_REQUEST);
	if (bytes == 0)
		client->SetStatus(Clients::CLOSED);
	if (bytes == -1)
	{
		if (client->GetRecv() > 10)
			client->SetStatus(Clients::CLOSED);
		client->SetRecv(client->GetRecv() + 1);
	}
}

void HTTPServer::HandleAfterReading(std::vector<char>& request, Clients* client)
{
	request = client->GetReadBuffer();
	if (request.empty())
		return (client->SetStatus(Clients::WAITING_REQUEST));
	if (client->_body.IsBody(client->_head))
	{
		request.erase(request.begin(), request.begin() + client->_head.GetIndexEndHeader() + 1);
		if (client->_body.GetChunk() == true)
		{
			if (client->_body.ParseChunk(request) == -1)
				return (client->_head.SetForError(true, 400), client->SetStatus(Clients::SENDING_RESPONSE));
		}
		else
			client->_body.SetBody(request);
	}
	client->SetStatus(Clients::SENDING_RESPONSE);
}

// void	HTTPServer::HandleExcevCGI(Epoll& epoll, Clients* client, int i)
// {

// }

void	HTTPServer::HandleCGI(Epoll& epoll, Clients* client, int i)
{
	if (client->GetCgiStatus() == Clients::CGI_NONE && client->_cgi.CheckCGI(client->_head, client->_body, servers[client->GetServerIndex()]))
		client->SetCgiStatus(Clients::CGI_AVAILABLE);
	if (client->GetCgiStatus() == Clients::CGI_AVAILABLE)
	{
		int state = client->_cgi.Execution(client->_head, client->_body, client->GetSocketServer(), epoll);
		if (state == 0)
		{
			this->_socket_client[client->_cgi.GetFdIn()] = client;
			this->_socket_client[client->_cgi.GetFdOut()] = client;
			client->SetCgiStatus(Clients::CGI_EXECUTING);
		}
		else if (state > 0)
		{
			client->_head.SetForError(true, state);
			client->SetCgiStatus(Clients::CGI_NONE);
		}
	}
	if (client->GetCgiStatus() == Clients::CGI_EXECUTING)
	{
		if (Timeout(client->_cgi.GetTimeBeginCGI(), 10))
		{
			// std::cout << "dans time out " << std::endl;
			client->_cgi.KillAndClose();
			client->_head.SetForError(true, 504);
			client->SetCgiStatus(Clients::CGI_NONE);
			client->_cgi.SetCgibody("");
			return ;
		}
		if (epoll.getEvent(i).events & EPOLLOUT && client->_cgi.GetWrote() == false)
		{
			// std::cout << "fd epollout= " << epoll.getEvent(i).data.fd << std::endl;
			// std::map<int, Clients*>::iterator it = this->_socket_client.find(epoll.getEvent(i).data.fd);
			// if (it != this->_socket_client.end())
				// std::cout << "client = " << it->second << std::endl;

			int state = client->_cgi.Write(client->_body);
			// std::cout << "dans write " << state << std::endl;
			if (state == 0)
				client->_cgi.SetWrote(true);
			if (state > 0)
			{
				client->_head.SetForError(true, state);
				client->SetCgiStatus(Clients::CGI_NONE);
			}
			// std::cout << "client->_cgi.GetWrote()" << client->_cgi.GetWrote() << "client->_cgi.GetRead()" << client->_cgi.GetRead() << std::endl;
		}
		if ((epoll.getEvent(i).events & EPOLLIN || epoll.getEvent(i).events & EPOLLHUP) && client->_cgi.GetRead() == false)
		{
			// std::cout << "fd epollin= " << epoll.getEvent(i).data.fd << std::endl;
			// std::map<int, Clients*>::iterator it = this->_socket_client.find(epoll.getEvent(i).data.fd);
			// if (it != this->_socket_client.end())
				// std::cout << "client = " << it->second << std::endl;

			// std::cout << "dans read "  << std::endl;
			int state = client->_cgi.Read(epoll);
			if (state == 0)
					client->_cgi.SetRead(true);
			else if (state > 0)
			{
				client->_head.SetForError(true, state);
				client->SetCgiStatus(Clients::CGI_NONE);
			}
			// std::cout << "client->_cgi.GetWrote()" << client->_cgi.GetWrote() << "client->_cgi.GetRead()" << client->_cgi.GetRead() << std::endl;
		}
		
		if (client->_cgi.GetWrote() == true && client->_cgi.GetRead() == true)
			client->SetCgiStatus(Clients::CGI_FINISHED);
	}
	if (client->GetCgiStatus() == Clients::CGI_FINISHED)
	{
		std::cout << "body :\n" << client->_cgi.GetCgiBody() << std::endl;
		client->SetCgiStatus(Clients::CGI_NONE);
	}
}

void HTTPServer::handleRequest(Epoll& epoll, int i, Clients* client)
{
	std::vector<char> request;
	int client_fd = epoll.getEvent(i).data.fd;
	if (epoll.getEvent(i).events & EPOLLIN && client->GetStatus() == Clients::WAITING_REQUEST)
	{
		std::cout << "------------REQUEST------------" << client_fd << std::endl;
		ReadAllRequest(client, client_fd);
	}
	if (client->GetStatus() == Clients::PARSING_REQUEST)
		HandleAfterReading(request, client);
	if (client->GetStatus() == Clients::SENDING_RESPONSE)
	{
		std::cout << "client->GetCgiStatus(): " << client->GetCgiStatus() << std::endl;
		client->SetLastActivity();
		if (client->_head.GetError() == 0)
			HandleCGI(epoll, client, i);
		if (client->GetCgiStatus() == Clients::CGI_NONE)
		{
			Response resp(this->servers[client->GetServerIndex()], client);
			if (resp.sendResponse(this->servers[client->GetServerIndex()], client, request) == 0)
			{
				client->SetStatus(Clients::CLOSED);
				return ;
			}
			client->_cgi.SetCgibody("");
			client->ClearBuff();
			client->SetStatus(Clients::WAITING_REQUEST);
		}
	}
	if (client->GetStatus() == Clients::CLOSED)
		CleanClient(client_fd, epoll);
}

void	HTTPServer::AcceptRequest(Epoll& epoll, int j)
{
	int socket = accept(this->_socket_server[j].GetFd(), NULL, NULL);
	if (socket < 0)
	{
		std::cerr << "Failed to grab socket_client." << std::endl;
		return;
	}
	Clients*	client = new Clients(this->_socket_server[j], socket);
	this->_socket_client[socket] = client;
	if (epoll.SetEpoll(socket, EPOLLIN | EPOLLOUT) == 0)
	{
		std::cerr << "Error: client socket is not created" << std::endl;
		CleanClient(client->GetSocket(), epoll);
	}
}

Clients*	HTTPServer::FindClient(int fd, int & id)
{
	(void)id;
	std::map<int, Clients*>::iterator it = this->_socket_client.find(fd);
	if (it != this->_socket_client.end())
	{
		// it->second->SetSessionId(id);
		// id++;
		return (it->second);
	}
	else
		return NULL;
}

// void HTTPServer::CheckToDelete(Epoll& epoll)
// {
// 	std::vector<int>	to_delete;

// 	for (std::map<int, Clients*>::iterator it = this->_socket_client.begin(); it != this->_socket_client.end(); it++)
// 	{
// 		if (Timeout(it->second->GetLastActivity(), 60) == true) 
// 			to_delete.push_back(it->first);
// 	}
// 	for (size_t i = 0; i < to_delete.size(); i++)
// 		CleanClient(to_delete[i], epoll);
// }

void	HTTPServer::CleanClient(int client_fd, Epoll& epoll)
{
	epoll.deleteClient(client_fd);
	std::map<int, Clients*>::iterator it = this->_socket_client.find(client_fd);
	std::map<int, Clients*>::iterator same_it = it;
	if (it != this->_socket_client.end())
	{
		close(it->first);;
		if (UniqueClient(it, same_it) == true)
			delete it->second;
		this->_socket_client.erase(it);
	}
}

int HTTPServer::runServer()
{
	Epoll* epoll = NULL;
	Clients* client = NULL;

	try
	{
		epoll = new Epoll(this->_socket_server);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		delete epoll;
		return (1);
	}
	int	server_index;
	signal(SIGINT, handle_sigint);

	while(g_running)
	{
		int n = epoll->epollWait();
		// CheckToDelete(*epoll);
		for (int i = 0; i < n; i++)
		{

			int	used_socket = epoll->getEvent(i).data.fd;
			server_index = GetServerIndex(used_socket);
			if (server_index >= 0)
			{
				// std::cout << used_socket << std::endl;
				AcceptRequest(*epoll, server_index);
			}
			else 
			{
				client = FindClient(used_socket, this->_counter_id);
				if (client != NULL)
					handleRequest(*epoll, i, client);
			}
		}
	}
	delete epoll;
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
		close(this->_socket_server[i].GetFd());
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
		SocketServer socket_server(socket(AF_INET, SOCK_STREAM, 0), port, host, j);
		if (socket_server.GetFd() < 0)
		{
			std::cerr << "Cannot create socket" << std::endl;
			close(socket_server.GetFd());
			return 1;
		}

		sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = htonl(prepareAddrForHtonl(host));
		sockaddr.sin_port = htons(port);

		host_port.push_back(std::make_pair(host, port));
		
		if (bind(socket_server.GetFd(), (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		{
			std::cerr << "Failed to bind to port " << port << "." << std::endl;
			close(socket_server.GetFd());
			return 1;
		}

		if (listen(socket_server.GetFd(), 10))
		{
			std::cerr << "Failed to listen on socket." << std::endl;
			close(socket_server.GetFd());
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
