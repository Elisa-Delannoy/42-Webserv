#ifndef PARSEBODYREQUEST_HPP
# define PARSEBODYREQUEST_HPP

# include "ParseRequest.hpp"
# include <map>
# include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>

class ParseBody
{
	private:
		std::string	_type;
		int			_len;

	public:
		ParseBody();
		~ParseBody();
		
		int		FindBodyLen(ParseRequest& request);
		void	ChooseContent(std::vector<char> body);
		void	AppForm(char* body_req);
		void	AppJson(std::vector<char> body);
		void	AppMultipart(std::vector<char>& body);
		
		
		struct Part
		{
			std::string	type;
			std::string	name;
			std::string	filename;
			std::vector<char>	content;

			friend std::ostream& operator<<(std::ostream& out, const Part& parts)
			{
				out << "Type: " << parts.type << " Name: " << parts.name << ", Filename: " << parts.filename << ", Content: ";
				if (!parts.content.empty())
				{
					for(std::vector<char>::const_iterator it = parts.content.begin(); it != parts.content.end(); it++)
					{
						// out << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)*(it);
						out << *it;
					}
					out << std::endl;
				}
				return (out);
			}
		};
		


		int GetContentLen() const;
		std::string GetContentType() const;
};


#endif