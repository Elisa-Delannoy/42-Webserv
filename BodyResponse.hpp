#ifndef BODYRESPONSE_HPP
# define BODYRESPONSE_HPP

#include "Response.hpp"

class BodyResponse : public Response
{
	public:
		BodyResponse(ServerConf & servers, Clients* client);
		~BodyResponse();

		void sendBody(HeaderResponse & header);
		int checkBody(const char* path);

		bool				getHasFilename() const;
		std::string			getFilename() const;
		std::vector<char>	getContent();

		void findBoundary(std::vector<char> & request);
		void findFilename(std::vector<char> & request);
		void findContent(std::vector<char> & request);

		std::string _body;

	private:
		std::vector<char> _content;
		std::string	_filename;
		std::string	_boundary;
		bool		_has_filename;
};

#endif