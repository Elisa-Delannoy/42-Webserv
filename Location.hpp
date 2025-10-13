#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstdlib>
#include <map>

class Location
{
	private:
		std::string _name;
		std::string _root;
		std::vector<std::string> _methods;
		bool _autoindex;
		std::map<std::string, std::string> _cgi;
		std::string _index;
	public:
		int nb_methods;

		Location();
		~Location();

		void SetName(std::string name);
		void SetRoot(std::string root);
		void SetMethods(std::string methods);
		void SetAutoindex(bool autoindex);
		void SetCGIPass(std::string ext, std::string path);
		void SetIndex(std::string index);

		std::string GetName() const;
		std::string GetRoot() const;
		std::string GetMethods(int nb) const;
		bool GetAutoindex() const;
		std::string GetCGIPass(std::string ext) const;
		std::map<std::string, std::string> GetCgi() const;
		std::string GetIndex() const;

		int AddName(std::string line);
		int AddRoot(std::string line);
		int AddMethods(std::string line);
		int AddAutoindex(std::string line);
		int AddCGIPass(std::string line);
		int AddIndex(std::string line);

		bool CheckMethod(std::string method);
};



