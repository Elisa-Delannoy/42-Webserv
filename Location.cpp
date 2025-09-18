#include "Location.hpp"

Location::Location()
{

}

Location::~Location()
{

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

void Location::AddRoot(Location& location, std::string line)
{

}

void Location::AddMethods(Location& location, std::string line)
{

}

void Location::AddAutoindex(Location& location, std::string line)
{

}
