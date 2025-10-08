#include "ParseRequest.hpp"	
#include "ParseBody.hpp"	
#include <iostream>
#include <sstream>
#include <string>

ParseRequest::ParseRequest() : _error(0)
{
}

ParseRequest::~ParseRequest()
{

}

const std::string&	ParseRequest::GetMethod() const
{
	return (this->_method);
}

const std::string&	ParseRequest::GetPath() const
{
	return (this->_path);
}

const std::string&	ParseRequest::GetVersion() const
{
	return (this->_version);
}

const std::string&	ParseRequest::GetNameLocation() const
{
	return (this->_name_location);
}

const std::map<std::string, std::string>&	ParseRequest::GetHeader() const
{
	return (this->_header);
}

int ParseRequest::DivideFirstLine(std::string& first_line)
{
	int len = first_line.length();
	std::istringstream	ss_first_line(first_line);
	ss_first_line >> this->_method >> this->_path >> this->_version;
	this->_name_location = "/";
	int len_w = this->_method.length() + this->_path.length() + this->_version.length();
	size_t sep = this->_path.substr(1).find('/');
	if (sep != std::string::npos)
		this->_name_location = this->_path.substr(0, sep + 1);
	if (len - len_w != 2)
		return (0); /* to do VOIR POUR LES RETOURS ERREUR*/
	return (1);
}

void ParseRequest::DivideHeader(std::string& line)
{
	std::string	name;
	std::string	content;
	size_t		delimiter = line.find_first_of(':');
	if (delimiter != std::string::npos)
	{
		name = line.substr(0, static_cast<int>(delimiter));
		content = line.substr(static_cast<int>(delimiter) + 1, line.size());
		this->_header[name]=content;
	}
}

int	ParseRequest::GetIndexEndHeader() const
{
	return (this->_end_header);
}

void	ParseRequest::SetIndexEndHeader(int index)
{
	this->_end_header = index;
}

int	ParseRequest::GetError() const
{
	return (this->_error);
}

void	ParseRequest::SetError(int code)
{
	this->_error = code;
} 
