#include "HTTPServer.hpp"
#include "ParseRequest.hpp"	
#include "ParseBodyRequest.hpp"	
#include <iostream>
#include <sstream>
#include <string>

// ParseRequest::ParseRequest()
// {

// }

ParseRequest::ParseRequest(char* req) : _request(req)
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

const std::map<std::string, std::string>&	ParseRequest::GetHeader() const
{
	return (this->_header);
}


void ParseRequest::DivideRequest()
{
	std::string 		string_request= this->_request;
	std::istringstream	ss_request(string_request);
	std::string 		line;
	bool				body = false;

	std::getline(ss_request, line);
	if (DivideFirstLine(line) == 0)
		return ;
	while (std::getline(ss_request, line))
	{
		if (!line.empty() && line.at(line.size() - 1) == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
		{
			body = true;
			std::string	body_type = this->_header["Content-Type"];
			std::istringstream	ss_body_len(this->_header["Content-Length"]);
			int	body_len;
			if (!(ss_body_len >> body_len) || !ss_body_len.eof() || body_len < 0)
				return;
			ParseBody	body(body_type, body_len);
			body.ChooseContent(ss_request);
			break;
		}
		if (body == false)
			DivideHeader(line);
	}
}

int ParseRequest::DivideFirstLine(std::string& first_line)
{
	int len = first_line.length() - 1;
	std::istringstream	ss_first_line(first_line);
	ss_first_line >> this->_method >> this->_path >> this->_version;
	int len_w = this->_method.length() + this->_path.length() + this->_version.length();
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
