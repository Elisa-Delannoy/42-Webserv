#include "Location.hpp"

Location::Location()
{
	_name = "";
	_root = "";
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
	nb_methods++;
}

void Location::SetAutoindex(bool autoindex)
{
	_autoindex = autoindex; 
}

void Location::SetCGIPass(std::string ext, std::string path)
{
	_cgi[ext] = path;
}

void Location::SetIndex(std::string index)
{
	_index = index;
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

std::string Location::GetCGIPass(std::string ext) const
{
	std::map<std::string, std::string>::const_iterator it = _cgi.begin();
	for (; it != _cgi.end(); it++)
	{
		if (it->first == ext)
			return it->second;
	}
	return "";
}

std::map<std::string, std::string> Location::GetCgi() const
{
	return _cgi;
}

std::string Location::GetIndex() const
{
	return _index;
}

int Location::AddName(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	ss >> word;
	if (word != "location")
		return 9;
	ss >> word;
	if (word == "~" || word == "=" || word == "^~" || word == "~*")
		return 6;
	SetName(word);
	return 0;
}

int Location::AddRoot(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	if (line[line.length() - 1] != ';')
		return 1;
	ss >> word;
	if (word != "root")
		return 9;
	ss >> word;
	word.erase(word.length() - 1);
	this->SetRoot(word);
	return 0;
}

int Location::AddMethods(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	if (line[line.length() - 1] != ';')
		return 1;
	ss >> word;
	if (word != "allow_methods")
		return 9;
	while (ss >> word)
	{
		if (word != "GET" && word != "POST" && word != "DELETE")
			break;
		SetMethods(word);
	}
	if (word != "GET;" && word != "POST;" && word != "DELETE;")
		return 7;
	word.erase(word.length() - 1);
	SetMethods(word);
	return 0;
}

int Location::AddAutoindex(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	if (line[line.length() - 1] != ';')
		return 1;
	ss >> word;
	if (word != "autoindex")
		return 9;
	ss >> word;
	if (word != "off;" && word != "on;")
		return 8;
	word.erase(word.length() - 1);
	if (word == "on")
		SetAutoindex(true);
	return 0;
}

int Location::AddCGIPass(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	if (line[line.length() - 1] != ';')
		return 1;
	ss >> word;
	if (word != "cgi")
		return 9;
	ss >> word;
	if (word != ".php" && word != ".py")
		return 9;
	std::string ext = word;
	ss >> word;
	if (word[word.length() - 1] != ';')
		return 10;
	word.erase(word.length() - 1);
	SetCGIPass(ext, word);
	return 0;
}

int Location::AddIndex(std::string line)
{
	std::istringstream ss(line);
	std::string word;

	if (line[line.length() - 1] != ';')
		return 1;
	ss >> word;
	if (word != "index")
		return 9;
	ss >> word;
	if (word.find(".") == std::string::npos)
		return 9;
	word.erase(word.length() - 1);
	SetIndex(word);
	return 0;
}
