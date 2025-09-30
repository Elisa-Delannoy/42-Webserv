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

int ServerConf::AddServerName(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "server_name")
		return 9;
	while (ss >> word)
	{
		std::string temp = word;
		if (!temp.empty() && temp[(temp.length() - 1)] == ';')
		{
			temp.erase(temp.length() - 1);
			this->SetServerName(temp);
			break;
		}
		this->SetServerName(temp);
	}
	if (word[(word.length() - 1)] != ';')
		return 1;
	return 0;
}

bool CheckHost(std::string host)
{
	struct in_addr addr;
	return inet_pton(AF_INET, host.c_str(), &addr) == 1;
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

int ServerConf::AddHostPort(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "listen")
		return 9;
	ss >> word;
	std::string temp = word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	int sep = word.find(':');
	if (CheckHost(word.substr(0, sep)) == false)
		return 3;
	int port = CheckValue(word.substr(sep + 1, word.length()).c_str());
	if (port == -1)
		return 2;
	if (temp[(temp.length() - 1)] != ';')
		return 1;
	this->SetHostPort(word.substr(0, sep), port);
	return 0;
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
		value = -1;
		break;
	}
	return value;
}

int ServerConf::AddClientBody(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word.compare("client_max_body_size") != 0)
		return 9;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] != ';')
		return 1;
	word.erase(word.length() - 1);
	if (word.length() <= 0)
		return 4;
	int temp = atoi(word.substr(0, word.length() - 1).c_str());
	char multiplier = word.substr(word.length() - 1).c_str()[0];
	int value = ClientBodyValue(temp, multiplier);
	if (value == -1)
		return 4;
	this->SetClientBodySize(value);
	return 0;
}

int ServerConf::AddErrorPage(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "error_page")
		return 9;
	ss >> word;
	int type_error = CheckValue(word);
	if (type_error == -1)
		return 5;
	ss >> word;
	if (word[(word.length() - 1)] != ';')
		return 1;
	word.erase(word.length() - 1);
	this->SetErrorPage(type_error, word);
	return 0;
}

int ServerConf::AddLocation(std::ifstream& conf, std::string line)
{
	Location location;
	location.AddName(line);
	while (line.find("}") == std::string::npos)
	{
		int error = 0;
		std::getline(conf, line);
		if (line.find("root") != std::string::npos)
			location.AddRoot(line);
		else if (line.find("allow_methods") != std::string::npos)
			location.AddMethods(line);
		else if (line.find("autoindex") != std::string::npos)
			location.AddAutoindex(line);
		else if (line.find("cgi_pass") != std::string::npos)
			location.AddCGIPass(line);
		else if (isComment(line) == true)
			continue;
		else
			error = 9;
	}
	this->SetLocation(location);
	_nb_location++;
	return 0;
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

std::string ServerConf::removeInlineComment(std::string &line)
{
	size_t pos = line.find('#');
	if (pos != std::string::npos)
		return line.substr(0, pos);
	return line;
}

std::string trimLeft(const std::string &s)
{
	size_t start = 0;
	while (start < s.size() && std::isspace(s[start]))
		start++;
	return s.substr(start);
}

bool ServerConf::isComment(const std::string &line)
{
	std::string trimmed = trimLeft(line);
	return trimmed.empty() || trimmed[0] == '#';
}

void ServerConf::Error(int error)
{
	if (error == 1)
		std::cerr << "Don't have ';' at the end of the line." << std::endl;
	if (error == 2)
		std::cerr << "Invalid Port." << std::endl;
	if (error == 3)
		std::cerr << "Invalid Host." << std::endl;
	if (error == 4)
		std::cerr << "Client body size value invalid." << std::endl;
	if (error == 5)
		std::cerr << "Invalid Error Page." << std::endl;
	if (error == 6)
		std::cerr << "Invalid line." << std::endl;
	if (error == 9)
		std::cerr << "Error Input." << std::endl;
}
