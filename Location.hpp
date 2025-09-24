#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstdlib>

class Location
{
	private:
		std::string _name;
		std::string _root;
		std::vector<std::string> _methods;
		bool _autoindex;
		std::string _cgi_pass;
	public:
		int nb_methods;

		Location();
		~Location();

		void SetName(std::string name);
		void SetRoot(std::string root);
		void SetMethods(std::string methods);
		void SetAutoindex(bool autoindex);
		void SetCGIPass(std::string cgi);

		std::string GetName() const;
		std::string GetRoot() const;
		std::string GetMethods(int nb) const;
		bool GetAutoindex() const;
		std::string GetCGIPass() const;

		void AddName(std::string line);
		void AddRoot(std::string line);
		void AddMethods(std::string line);
		void AddAutoindex(std::string line);
		void AddCGIPass(std::string line);
};



