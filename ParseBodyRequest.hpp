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
		
		int		FindBodyLen(ParseRequest& request);
		void	ChooseContent(char* body);
		void	AppForm(std::istringstream& body);
		void	AppJson(std::istringstream& body);
		void	AppMultipart(std::istringstream& body);
		
		struct Part
		{
			std::string	name;
			std::string	filename;
			std::string	type;
			std::vector<char>	content;
		};
		

};


#endif