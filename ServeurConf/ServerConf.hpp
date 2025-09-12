#ifndef _SERVERCONF_
	#define _SERVEURCONF_

#include <iostream>
#include <string>
#include <map>

class ServerConf
{
	private:
		std::string _server_name;
		std::string _host;
		int _port;
		int _client_body_size;
		std::map<int, std::string> _error;
	public:
		ServerConf();
		~ServerConf();
		
		void SetServerName(std::string server_name);
		void SetHost(std::string host);
		void SetPort(int port);
		void SetClientBodySize(int size);
};

#endif