#include "HTTPServer.hpp"
#include "ParseRequest.hpp"	
#include <iostream>
#include <sstream>

// ParseRequest::ParseRequest()
// {

// }

ParseRequest::ParseRequest(char* req) : _request(req)
{

}

ParseRequest::~ParseRequest()
{

}

void ParseRequest::DivideRequest()
{
	std::string 		string_request= this->_request;
	std::istringstream	ss_request(string_request);
	std::string 		first_line;

	std::getline(ss_request, first_line);
	std::cout << first_line << std::endl;
	DivideFirstLine(first_line);


}

void ParseRequest::DivideFirstLine(std::string first_line)
{
	int len = first_line.length() - 1;
	std::istringstream	ss_first_line(first_line);
	ss_first_line >> this->_method >> this->_path >> this->_version;
	int len_w = this->_method.length() + this->_path.length() + this->_version.length();
	// std::cout << "methode = " << this->_method << " path = " << this->_path <<" verison = " << this->_version << std::endl;
	// std::cout << "methode = " << this->_method.length() << " path = " << this->_path.length() <<" verison = " << this->_version.length() << std::endl;
	// std::cout << len << " | " << len_w << std::endl;
	
	if (len - len_w != 2)
		return;

}
