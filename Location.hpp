#pragma once

#include "ServerConf.hpp"

class Location : public ServerConf
{
	private:
		std::string _root;
		std::vector<std::string> _methods;
		bool _autoindex;
		std::string _cgi_pass;
	public:
		Location();
		~Location();

		void SetRoot(std::string root);
		void SetMethods(std::string methods);
		void SetAutoindex(bool autoindex);

		std::string GetRoot() const;
		std::string GetMethods(int nb) const;
		bool GetAutoindex() const;

		void AddRoot(Location& location, std::string line);
		void AddMethods(Location& location, std::string line);
		void AddAutoindex(Location& location, std::string line);
};



