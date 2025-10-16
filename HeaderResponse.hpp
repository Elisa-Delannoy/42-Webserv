#ifndef HEADERRESPONSE_HPP
# define HEADERRESPONSE_HPP

#include "Response.hpp"

class HeaderResponse : public Response
{
	public:
		HeaderResponse(ServerConf & servers, Clients* client, std::string & path, std::string version);
		~HeaderResponse();

		void 		sendHeader(bool has_body, bool & to_close);

		//SETTING HEADER
		void 		setHeader(int code, std::vector<std::string> & methods);
		std::string setStatus(std::string code);
		std::string setContentType();
		std::string setSize(const char* path_image);
		std::string setContentLength();
		std::string setConnection(Clients* client);
		void		setCloseAlive(int x);

		int 		getCloseAlive();
		std::string getValueHeader(Clients* client, std::string key);

		void setPath(std::string path);

	private:
		std::string _header;
		std::string & _path;
		std::string _version;
		std::string _connection;
		std::string _allow;
		std::string _accept;
		std::string _cookie;
		int 		_close_alive;
};

#endif