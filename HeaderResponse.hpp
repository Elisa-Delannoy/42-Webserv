#ifndef HEADERRESPONSE_HPP
# define HEADERRESPONSE_HPP

#include "Response.hpp"

class HeaderResponse : public Response
{
	public:
		HeaderResponse(ServerConf & servers, Clients* client, std::string & path, std::string version);
		~HeaderResponse();

		void sendHeader();
		
		//SETTING HEADER
		void setHeader(int code);
		std::string setStatus(std::string code);
		std::string setContentType();
		std::string setSize(const char* path_image);
		std::string setContentLength();

	private:
		std::string _header;
		std::string & _path;
		std::string _version;
};

#endif