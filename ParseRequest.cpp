#include "ParseRequest.hpp"	
#include "ParseBodyRequest.hpp"	
#include <iostream>
#include <sstream>
#include <string>

ParseRequest::ParseRequest()
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
	// std::cout << "|" << first_line << "|" << std::endl;
	int len = first_line.length();
	std::istringstream	ss_first_line(first_line);
	ss_first_line >> this->_method >> this->_path >> this->_version;
	this->_name_location = "/";
	int len_w = this->_method.length() + this->_path.length() + this->_version.length();
	size_t sep = this->_path.substr(1).find('/');
	if (sep != std::string::npos)
		this->_name_location = this->_path.substr(0, sep + 1);
	// std::cout << "methode = " << this->_method << " path = " << this->_path <<" verison = " << this->_version << std::endl;
	// std::cout << "methode = " << this->_method.length() << " path = " << this->_path.length() <<" verison = " << this->_version.length() << std::endl;
	// std::cout << len << " | " << len_w << std::endl;
	
	if (len - len_w != 2)
		return (0); /* VOIR POUR LES RETOURS ERREUR*/
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
