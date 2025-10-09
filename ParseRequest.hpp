#ifndef PARSEREQUEST_HPP
# define PARSEREQUEST_HPP

# include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class ParseRequest
{
	private:
		int									_error;
		int									_end_header;
		bool								_to_close;
		std::string							_path;
		std::string							_method;
		std::string							_version;
		std::string							_name_location;
		std::map<std::string, std::string>	_header;

	public:
		ParseRequest();
		~ParseRequest();

		void	DivideHeader(std::string& first_line);
		int		DivideFirstLine(std::string& first_line);
		
		int	GetError() const;
		int	GetIndexEndHeader() const;
		bool	GetToClose() const;
		const std::string&	GetPath() const;
		const std::string&	GetMethod() const;
		const std::string&	GetVersion() const;
		const std::string&	GetNameLocation() const;
		const std::map<std::string, std::string>&	GetHeader() const;

		// void	SetError(int index);
		void	SetIndexEndHeader(int index);
		void	SetForError(bool to_close, int code);
		// void	SetToClose(bool status);
};

#endif
