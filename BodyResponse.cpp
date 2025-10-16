#include "BodyResponse.hpp"
#include "HeaderResponse.hpp"

BodyResponse::BodyResponse(ServerConf & servers, Clients* client) :
Response(servers, client), _has_filename(false)
{ }

BodyResponse::~BodyResponse()
{ }

void BodyResponse::sendBody(HeaderResponse & header)
{
	size_t total_sent = 0;
	const char* buffer = this->_body.data();
	size_t to_send = this->_body.size();

	while (total_sent < to_send)
	{
		ssize_t sent = send(this->_client_fd, buffer + total_sent, to_send - total_sent, 0);

		if (sent <= 0)
		{
			if (sent == 0)
				std::cerr << "Client closed connection while sending body." << std::endl;
			else
				std::cerr << "Error while sending body: " << strerror(errno) << std::endl;
			header.setCloseAlive(0);
			break;
		}
		total_sent += sent;
	}
}

//Return 0 if ok
//Return 1 if reading problem
//Return 404 if file problem
int BodyResponse::checkBody(const char* path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) //wrong path, invalid rights, inexisting file
		return 404;

	std::stringstream buffer;
	buffer << file.rdbuf();
	if (file.fail()) //reading problem
		return 1;

	file.close();
	this->_body = buffer.str();
	return 0;
}

void BodyResponse::findBoundary(std::vector<char> & request)
{
	std::string str = "\r\n";
	std::string s(request.begin(), request.end());
	size_t i = s.find(str);
	if (i != std::string::npos)
	{
		this->_boundary = s.substr(0, i);
	}
}

void BodyResponse::findFilename(std::vector<char> & request)
{
	std::string str = "filename";
	 if (request.size() < request.size())
		this->_has_filename = false;

	std::string s(request.begin(), request.end());
	size_t index = s.find(str);
	if (index != std::string::npos)
	{
		index += 10;
		size_t end_filename = index;
		while(end_filename < request.size() && request[end_filename] != '"')
		end_filename++;
		this->_filename = s.substr(index, end_filename - index);
		if (!this->_filename.empty())
			this->_has_filename = true;
	}
	else
		this->_has_filename = false;
}

void BodyResponse::findContent(std::vector<char> & request)
{
	std::string s(request.begin(), request.end());
	size_t found = s.find("Content-Type");
	size_t index = 0;
	while(index < found)
		index = s.find("\r\n\r\n", index+4);
	if (index != std::string::npos)
	{
		index += 4;
		std::string s2(request.begin() + index, request.end());
		size_t end_content = s2.find(this->_boundary);
		this->_content.insert(this->_content.end(), s2.begin(), s2.begin() + end_content);
	}
}

bool BodyResponse::getHasFilename() const
{
	return this->_has_filename;
}

std::string BodyResponse::getFilename() const
{
	return this->_filename;
}

std::vector<char> BodyResponse::getContent()
{
	return this->_content;
}
