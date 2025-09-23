#ifndef PARSEBODYREQUEST_HPP
# define PARSEBODYREQUEST_HPP

# include "HTTPServer.hpp"
# include "ParseRequest.hpp"
# include <map>

class ParseBody
{
	private:
		std::string	_type;
		int			_len;

	public:
		ParseBody();
		~ParseBody();
		void	ChooseContent(char* body);
		void	AppForm(std::istringstream& body);
		void	AppJson(std::istringstream& body);
		int		FindBodyLen(ParseRequest& request);
};


#endif