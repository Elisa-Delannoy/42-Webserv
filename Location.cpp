#include "Location.hpp"

Location::Location()
{
	_name = "";
	_root = "";
	_cgi_pass = "";
	_autoindex = false;
	nb_methods = 0;
}

Location::~Location()
{
}

void Location::SetName(std::string name)
{
	_name = name;
}

void Location::SetRoot(std::string root)
{
	_root = root;
}

void Location::SetMethods(std::string methods)
{
	_methods.push_back(methods);
}

void Location::SetAutoindex(bool autoindex)
{
	_autoindex = autoindex; 
}

void Location::SetCGIPass(std::string cgi)
{
	_cgi_pass = cgi;
}

std::string Location::GetName() const
{
	return _name;
}

std::string Location::GetRoot() const
{
	return _root;
}

std::string Location::GetMethods(int nb) const
{
	return _methods[nb];
}

bool Location::GetAutoindex() const
{
	return _autoindex;
}

std::string Location::GetCGIPass() const
{
	return _cgi_pass;
}

int Location::AddName(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "location")
		return 9;
	ss >> word;
	if (word == "~") /*check  autre types*/
		ss >> word;
	SetName(word);
	return 0;
}

int Location::AddRoot(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "root")
		return 9;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	this->SetRoot(word);
	return 0;
}

int Location::AddMethods(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "allow_methods")
		return 9;
	while (true)
	{
		std::string temp = word;
		ss >> word;
		if (temp == word)
			return 9;
		if (!word.empty() && word[(word.length() - 1)] == ';')
			word.erase(word.length() - 1);
		if (word == "GET" || word == "DELETE" || word == "POST")
		{
			SetMethods(word);
			nb_methods++;
		}
		else
			return 9;
	}
	return 0;
}

int Location::AddAutoindex(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "autoindex")
		return 9;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	if (word == "on")
		SetAutoindex(true);
	return 0;
}

int Location::AddCGIPass(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "cgi_pass")
		return 9;
	ss >> word;
	if (!word.empty() && word[(word.length() - 1)] == ';')
		word.erase(word.length() - 1);
	SetCGIPass(word);
	return 0;
}
