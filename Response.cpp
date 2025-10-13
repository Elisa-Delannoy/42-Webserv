#include "Response.hpp"
#include "HeaderResponse.hpp"
#include "BodyResponse.hpp"
#include <cerrno>

Response::Response(ServerConf & servers, Clients* client) : _server(servers)
{
	this->_errors_path = this->_server.GetErrorPath();
	this->_body_len = client->_body.GetContentLen();
	this->_client_fd = client->GetSocket();
}

Response::~Response()
{ }

//Loop through locations from conf file to find correct root
//Also set allowed methods (GET/POST/DELETE)
void Response::setRootLocationAndMethods(std::string & path)
{
	std::string name;
	this->_methods.clear();

	for (int i = 0; i < this->_server._nb_location; i++)
	{
		name = this->_server.GetLocation(i).GetName();
		if (name == "/")
			this->_index_location = i;
		if (!name.empty() && name != "/" && path.compare(0, name.size(), name) == 0)
		{
			//add allowed methods
			for (int j = 0; j < this->_server.GetLocation(i).nb_methods; j++)
				this->_methods.push_back(this->_server.GetLocation(i).GetMethods(j));

			this->_index_location = i;
			this->_root = this->_server.GetLocation(i).GetRoot() + "/";
			this->_root.erase(this->_root.begin(), this->_root.begin()+1);
			path.replace(0, name.size(), this->_root);
			return;
		}
	}
	if (this->_root.empty())
	{
		//add allowed methods
		for (int j = 0; j < this->_server.GetLocation(this->_index_location).nb_methods; j++)
			this->_methods.push_back(this->_server.GetLocation(this->_index_location).GetMethods(j));

		this->_root = this->_server.GetLocation(this->_index_location).GetRoot();
		this->_root.erase(this->_root.begin(), this->_root.begin()+1);
		path.replace(0, name.size() - 1, this->_root);
	}
}

/*
	(code == 400)
	(code == 405)
*/

void Response::sendError(HeaderResponse & header, BodyResponse & body, int code)
{
	header.setHeader(code, this->_methods);
	std::cout << "this->_errors_path.find(code) " << this->_errors_path[code] << std::endl;
	if (this->_errors_path[code].empty())
	{
		if (code == 400)
			body._body = ERROR400;
		if (code == 404)
			body._body = ERROR404;
		if (code == 405)
			body._body = ERROR405;
		if (code == 500)
			body._body  = ERROR500;

		std::ostringstream oss;
		oss << body._body.size();
		header._content_length = "Content-Length: " + oss.str() + "\r\n";
	}
	else
	{
		std::string path = GetErrorPath(code).c_str();
		header.setPath(path);
		if (body.checkBody(path.c_str()) == 0)
			header._content_length = header.setContentLength();
		else
		{
			header.setHeader(500, this->_methods);
			header.sendHeader(false);
			return ;
		}
	}
	sendHeaderAndBody(header, body);
}

void Response::displayAutoindex(HeaderResponse & header, BodyResponse & body, std::string path)
{
	DIR *dir;
	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		std::cout << "path opendir error : " << path << std::endl;
		header.setHeader(404, this->_methods);
		sendError(header, body, 404);
	}
	else
	{
		struct dirent* dirp;
		body._body = "<html><body><h1>AUTOINDEX</h1><ul>";
		dirp = readdir(dir);
		while (dirp != NULL)
		{
			std::string name = dirp->d_name;
			size_t found = name.find(".");
			if (found == std::string::npos)
				name += "/";
			std::cout << "path : " << path << std::endl;
			if (path == "uploads/")
				body._body += "<li><a href=\"" + path + name + "\">" + name + "</a></li>";
			else
				body._body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
			dirp = readdir(dir);
		}
		body._body += "</ul></body></html>";
		header._body_len = body._body.size();
		header._content_length = header.setContentLength();
		header.setHeader(200, this->_methods);
		sendHeaderAndBody(header, body);
	}
	closedir(dir);
}

void Response::displayUploadSuccessfull(HeaderResponse & header, BodyResponse & body)
{
	body._body = "<html><body><h1>Upload Successfull!</h1></body></html>";
	header._body_len = body._body.size();
	header._content_length = header.setContentLength();
	header.setHeader(200, this->_methods);
	sendHeaderAndBody(header, body);
}

void Response::createFileOnServer(HeaderResponse & header, BodyResponse & body, std::vector<char> & request)
{
	DIR *dir;
	dir = opendir("uploads/");
	if (dir == NULL)
	{
		header.setHeader(404, this->_methods);
		header.sendHeader(false);
	}
	std::string filename = "uploads/" + body.getFilename();
	std::ofstream out(filename.c_str(), std::ios::binary);
	body.findBoundary(request);
	body.findContent(request);

	if (body._body.size() >= static_cast<size_t>(header._server.GetClientBodySize()))
	{
		out.close();
		header.setHeader(413, this->_methods);
		header.sendHeader(false);
	}
	else
	{
		out.write(body.getContent().data(), body.getContent().size());
		out.close();
		displayUploadSuccessfull(header, body);
	}
	closedir(dir);
}

//Return 0 if connection is closed
//Return 1 if connection is keep-alive
int Response::sendResponse(ServerConf & servers, Clients* client, std::vector<char> request)
{
	ExecCGI cgi;
	std::string path = client->_head.GetPath();
	std::string method = client->_head.GetMethod();
	std::string version = client->_head.GetVersion();
	std::cout << "|" << path << "|" << method << "|" << version << "|" << std::endl;
	
	/* std::cout << "\n\n--------BUF BEGIN--------" << std::endl;
	std::vector<char>::iterator it = request.begin();
	for(; it != request.end(); it++)
		std::cout << *it;
	std::cout << "\n--------BUF END-------\n" << std::endl; */

	setRootLocationAndMethods(path);
	HeaderResponse header(servers, client, path, version);
	BodyResponse body(servers, client);

	//error in header
	if (client->_head.GetError() != 0)
	{
		header.setHeader(client->_head.GetError(), this->_methods);
		sendHeader(header);
		return (header.getCloseAlive());
	}
	//if cgi > body._body = string  de noah
	//
	bool method_allowed = isMethodAllowed(method);
	if (method == "GET")
	{
		std::cout << "get method" << std::endl;
		if (method_allowed)
			handleGet(header, body, path);
		else
		{
			sendError(header, body, 405);
		}
	}
	else if (method == "POST")
	{
		std::cout << "post method" << std::endl;
		if (method_allowed)
		{
			std::string content_type = header.getValueHeader(client, "Content-Type");

			if (content_type.substr(0, 20) == " multipart/form-data")
			{
				body.findFilename(request);
				if (body.getHasFilename())
				{
					createFileOnServer(header, body, request);
				}
				else //try to upload an empty file
				{
					body._body = "<html><body><h1>Empty Upload</h1></body></html>";
					header._body_len = body._body.size();
					header._content_length = header.setContentLength();
					header.setHeader(200, this->_methods);
					sendHeaderAndBody(header, body);
				}
			}
			else
			{
				header.setHeader(200, this->_methods);
				header.sendHeader(false);
			}
		}
		else
		{
			sendError(header, body, 405);
		}
	}
	else if (method == "DELETE")
	{
		std::cout << "delete method" << std::endl;
		if (method_allowed)
		{
			if (access(path.c_str(), F_OK) != 0) //file not found
			{
				std::cout << "file not found" << std::endl;
				header.setHeader(404, this->_methods);
				header._content_length = "Content-Length: 16\r\n";
				body._body = "File not found";
				sendHeaderAndBody(header, body);
			}
			else
			{
				std::cout << "path.substr(0, 8)" << path.substr(0, 8) << std::endl;
				if (path.substr(0, 8) != "uploads/")
				{
					header.setHeader(403, this->_methods);
					header.sendHeader(false);
				}
				else
				{
					if (unlink(path.c_str()) == 0) //delete file
					{
						header.setHeader(204, this->_methods);
						header.sendHeader(false);
					}
					else //could not delete file
					{
						header.setHeader(404, this->_methods);
						header.sendHeader(false);
					}
				}
			}
		}
		else
		{
			sendError(header, body, 405);
		}
	}
	else
	{
		sendError(header, body, 405);
	}
	return (header.getCloseAlive());
}

void Response::handleGet(HeaderResponse & header, BodyResponse & body, std::string & path)
{
	int check;
	DIR *dir;
	dir = opendir(path.c_str());
	if (dir != NULL) //path is a dir
	{
		handlePathDir(header, body, path);
	}
	else //path is a file
	{
		check = body.checkBody(path.c_str());
		if (check == 0)
		{
			header.setHeader(200, this->_methods);
			sendHeaderAndBody(header, body);
		}
		else if (check == 404) //wrong path
		{
			sendError(header, body, 404);
		}
		else
		{
			sendError(header, body, 500);
		}
	}
	closedir(dir);
}

void Response::handlePathDir(HeaderResponse & header, BodyResponse & body, std::string & path)
{
	int check;
	std::string index = getIndex();

	if (index.empty())
	{
		bool autoindex = getAutoindex();
		if (autoindex)
		{
			displayAutoindex(header, body, path);
		}
		else
		{
			sendError(header, body, 404);
		}
	}
	else
	{
		path += index;
		check = body.checkBody(path.c_str());
		if (check == 0)
		{
			header.setHeader(200, this->_methods);
			sendHeaderAndBody(header, body);
		}
		else
		{
			sendError(header, body, 500);
		}
	}
}

bool Response::isMethodAllowed(std::string method)
{
	bool ret = false;
	for (size_t i = 0; i < this->_methods.size(); i++)
	{
		if(this->_methods[i] == method)
			ret = true;
	}
	return ret;
}

void Response::sendHeaderAndBody(HeaderResponse & header, BodyResponse & body)
{
	header.sendHeader(true);
	body.sendBody();
}

//Return index from conf file
std::string Response::getIndex()
{
	return this->_server.GetLocation(this->_index_location).GetIndex();
}

std::string Response::GetErrorPath(int code)
{
	return (this->_errors_path[code]);
}

bool Response::getAutoindex()
{
	return this->_server.GetLocation(this->_index_location).GetAutoindex();
}
