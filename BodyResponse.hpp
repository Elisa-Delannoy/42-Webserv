#ifndef BODYRESPONSE_HPP
# define BODYRESPONSE_HPP

#include "Response.hpp"

class BodyResponse : public Response
{
	public:
		BodyResponse(ServerConf & servers, Clients* client);
		~BodyResponse();

		void sendBody();
		int checkBody(const char* path);

		std::string _body;
};

#endif