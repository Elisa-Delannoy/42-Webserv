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
		ParseBody(std::string& type, int len);
		~ParseBody();
		void	ChooseContent(std::istringstream& body);
		void	AppForm(std::istringstream& body);
		void	AppJson(std::istringstream& body);

};


#endif