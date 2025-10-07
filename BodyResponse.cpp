#include "BodyResponse.hpp"

BodyResponse::BodyResponse(ServerConf & servers, Clients* client) :
Response(servers, client)
{ }

BodyResponse::~BodyResponse()
{ }

void BodyResponse::sendBody()
{
	size_t data_sent = 0;
	while(data_sent < this->_body.size())
	{
		ssize_t data_read = send(this->_client_fd, this->_body.data() + data_sent,
			this->_body.size() - data_sent, 0);
		if (data_read == -1)
		{
			std::cerr << "Error while sending content." << std::endl;
			break;
		}
		data_sent += data_read;
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
