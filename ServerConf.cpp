#include "ServerConf.hpp"

ServerConf::ServerConf()
{
	_client_body_size = 0;
	_nb_location = 0;
}

ServerConf::~ServerConf()
{
}

void ServerConf::SetServerName(std::string server_name)
{
	_server_name.push_back(server_name);
}

void ServerConf::SetHostPort(std::string host, int port)
{
	_host_port.push_back(std::make_pair(host, port));
}

void ServerConf::SetClientBodySize(int size)
{
	_client_body_size = size;
}

void ServerConf::SetErrorPage(int type_error, std::string path)
{
	_error_pages[type_error] = path;
}

void ServerConf::SetLocation(Location location)
{
	_location.push_back(location);
}

std::vector<std::string> ServerConf::GetServerName() const
{
	return _server_name;
}

int ServerConf::GetClientBodySize() const
{
	return _client_body_size;
}

int ServerConf::GetPort(int nb) const
{
	return _host_port[nb].second;
}

std::string ServerConf::GetHost(int nb) const
{
	return (_host_port[nb].first);
}

std::string ServerConf::GetErrorPath(int type_error)
{
	return (_error_pages[type_error]);
}

std::map<int, std::string> ServerConf::GetErrorPath()
{
	return _error_pages;
}

Location& ServerConf::GetLocation(int nb)
{
	return (_location[nb]);
}

size_t ServerConf::GetHostPortSize() const
{
	return this->_host_port.size();
}

void ServerConf::AddServerName(std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "server_name")
		return ;
	while (ss >> word)
	{
		if (!word.empty() && word[(word.length() - 1)] == ';')
		{
			word.erase(word.length() - 1);
			this->SetServerName(word);
			break;
		}
		this->SetServerName(word);
	}
}

int CheckValue(std::string port)
{
	int intport = atoi(port.c_str());
	std::ostringstream oss;
	oss << intport;
	if (oss.str() != port)
		return -1;
	return intport;
}

void ServerConf::AddHostPort(std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "listen")
		return ;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	int sep = word.find(':');
	int port = CheckValue(word.substr(sep + 1, word.length()).c_str());
	if (port == -1)
		return;
	this->SetHostPort(word.substr(0, sep), port);
}

int ClientBodyValue(int value, int multiplier)
{
	switch (multiplier)
	{
	case 'K':
		value = value * 1000;
		break;
	case 'M':
		value = value * 1000000;
		break;
	case 'G':
		value = value * 1000000000;
		break;
	default:
		break;
	}
	return value;
}

void ServerConf::AddClientBody(std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "client_max_body_size")
		return ;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	int value = atoi(word.substr(0, word.length() - 1).c_str());
	char multiplier = word.substr(word.length() - 1).c_str()[0];
	this->SetClientBodySize(ClientBodyValue(value, multiplier));
}

void ServerConf::AddErrorPage(std::string& line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "error_page")
		return ;
	ss >> word;
	int type_error = CheckValue(word);
	if (type_error == -1)
		return;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	this->SetErrorPage(type_error, word);
}

void ServerConf::AddLocation(std::ifstream& conf, std::string& line)
{
	Location location;
	location.AddName(line);
	while (line.find("}") == std::string::npos)
	{
		std::getline(conf, line);
		if (line.find("root") != std::string::npos)
			location.AddRoot(line);
		if (line.find("allow_methods") != std::string::npos)
			location.AddMethods(line);
		if (line.find("autoindex") != std::string::npos)
			location.AddAutoindex(line);
		if (line.find("cgi_pass") != std::string::npos)
			location.AddCGIPass(line);
	}
	this->SetLocation(location);
	_nb_location++;
}

bool ServerConf::checkMethods(std::string method, int nb)
{
	for (int i = 0; i < GetLocation(nb).nb_methods; i++)
	{
		if (GetLocation(nb).GetMethods(i) == method)
			return true;
	}
	return false;
}

int ServerConf::checkLocation(std::string name)
{
	for (int i = 0; i < _nb_location; i++)
	{
		if (GetLocation(i).GetName().find(name))
			return i;
	}
	return -1;
}
