#include "Response.hpp"
#include "HeaderResponse.hpp"
#include "BodyResponse.hpp"
#include <cerrno>

Response::Response(ServerConf & servers, Clients* client) : _server(servers)
{
	this->_errors_path = this->_server.GetErrorPath();
	this->_body_len = client->_body.GetContentLen();
	this->_client_fd = client->GetSocket();
	this->_to_close = client->_head.GetToClose();
	this->_begin_405 = "<html><head><title>405 Method Not Allowed</title></head><body><center><h1>405 Method Not Allowed</h1></center>";
	this->_end_405 = "<hr><center>CookieServ</center></body></html>";
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
		for (int j = 0; j < this->_server.GetLocation(this->_index_location).nb_methods; j++)
			this->_methods.push_back(this->_server.GetLocation(this->_index_location).GetMethods(j));

		this->_root = this->_server.GetLocation(this->_index_location).GetRoot();
		this->_root.erase(this->_root.begin(), this->_root.begin()+1);
		path.replace(0, name.size() - 1, this->_root);
	}
}

void Response::sendError(HeaderResponse & header, BodyResponse & body, int code)
{
	header.setHeader(code, this->_methods);
	if (this->_errors_path[code].empty())
	{
		if (code == 400)
			body._body = ERROR400;
		else if (code == 404)
			body._body = ERROR404;
		else if (code == 405)
		{
			this->_begin_405 += "<center><h3>Methods allowed : ";
			for(size_t i = 0; i < this->_methods.size(); i++)
			{
				this->_begin_405 += this->_methods[i];
				this->_begin_405 += " ";
			}
			this->_begin_405 += "</h3></center>";
			body._body = this->_begin_405 + this->_end_405;
		}
		else if (code == 408)
			body._body  = ERROR408;
		else if (code == 413)
			body._body  = ERROR413;
		else if (code == 500)
			body._body  = ERROR500;
		else if (code == 503)
			body._body  = ERROR503;
		else if (code == 504)
			body._body  = ERROR504;

		std::ostringstream oss;
		oss << body._body.size();
		header._content_length = "Content-Length: " + oss.str() + "\r\n";
	}
	else
	{
		std::string path = GetErrorPath(code).c_str();
		header.setPath(path);
		int check = body.checkBody(path.c_str());
		if (check == 0)
		{
			header._body_len = body._body.size();
			header._content_length = header.setContentLength();
		}
		else
		{
			if (check == 404)
			{
				header.setHeader(404, this->_methods);
				body._body  = ERROR404;
			}
			else
			{
				header.setHeader(500, this->_methods);
				body._body  = ERROR500;
			}
			std::ostringstream oss;
			oss << body._body.size();
			header._content_length = "Content-Length: " + oss.str() + "\r\n";
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
		if (errno == ENOENT)
			sendError(header, body, 404);
		else
			sendError(header, body, 500);
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
		if (errno == ENOENT)
			sendError(header, body, 404);
		else
			sendError(header, body, 500);
	}
	else
	{
		std::string filename = "uploads/" + body.getFilename();
		std::ofstream out(filename.c_str(), std::ios::binary);
		body.findBoundary(request);
		body.findContent(request);

		if (body._body.size() >= static_cast<size_t>(header._server.GetClientBodySize()))
		{
			out.close();
			sendError(header, body, 413);
		}
		else
		{
			out.write(body.getContent().data(), body.getContent().size());
			if (!out)
			{
				sendError(header, body, 500);
				return ;
			}	
			out.close();
			displayUploadSuccessfull(header, body);
		}
	}
	closedir(dir);
}

//Return 0 if connection is closed
//Return 1 if connection is keep-alive
int Response::sendResponse(ServerConf & servers, Clients* client, std::vector<char> request)
{
	std::string path = client->_head.GetPath();
	this->_path_unchanged = client->_head.GetPath();;
	std::string method = client->_head.GetMethod();
	std::string version = client->_head.GetVersion();
	std::cout << "|" << path << "|" << method << "|" << version << "|" << std::endl;

	setRootLocationAndMethods(path);
	HeaderResponse header(servers, client, path, version);
	BodyResponse body(servers, client);

	if (client->_head.GetError() != 0)
	{
		sendError(header, body, client->_head.GetError());
		return (header.getCloseAlive());
	}

	if (this->_path_unchanged == "/favicon.ico")
	{
		header.setHeader(204, this->_methods);
		header.sendHeader(this->_to_close);
		return 1;
	}

	if (this->_path_unchanged == "/.well-known/appspecific/com.chrome.devtools.json")
	{
		header.setHeader(404, this->_methods);
		header.sendHeader(this->_to_close);
		return 1;
	}

	if (this->_path_unchanged == "/redirect")
	{
		header.setRedirect();
		header.sendHeader(this->_to_close);
		return 1;
	}

	if (!client->_cgi.GetCgiBody().empty())
	{
		std::cout << "cgi" << std::endl;
		handleCgi(header, body, client);
		return 1;
	}

	bool method_allowed = isMethodAllowed(method);
	if (method == "GET")
	{
		std::cout << "get method" << std::endl;
		if (method_allowed)
			handleGet(header, body, path);
		else
			sendError(header, body, 405);
	}
	else if (method == "POST")
	{
		std::cout << "post method" << std::endl;
		if (method_allowed)
			handlePost(header, body, client, request);
		else
			sendError(header, body, 405);
	}
	else if (method == "DELETE")
	{
		std::cout << "delete method" << std::endl;
		if (method_allowed)
			handleDelete(header, body, path);
		else
			sendError(header, body, 405);
	}
	else
		sendError(header, body, 405);
	return (header.getCloseAlive());
}

void Response::handleCgi(HeaderResponse & header, BodyResponse & body, Clients* client)
{
	size_t pos = client->_cgi.GetCgiBody().find("\n\r\n");
	if (pos == std::string::npos)
		pos = client->_cgi.GetCgiBody().find("\n\n");
	if (pos == std::string::npos)
	{
		sendError(header, body, 500);
		return ;
	}
	body._body = client->_cgi.GetCgiBody().substr(pos);
	header._body_len = body._body.size();
	header.setHeaderCGI(client->_cgi.GetCgiBody().substr(0, pos));
	sendHeaderAndBody(header, body);
}

void Response::handleGet(HeaderResponse & header, BodyResponse & body, std::string & path)
{
	int check;
	DIR *dir;
	std::cout << "path : " << path << std::endl;
	dir = opendir(path.c_str());
	if (dir != NULL) //path is a dir
		handlePathDir(header, body, path);
	else //path is a file
	{
		check = body.checkBody(path.c_str());
		if (check == 0)
		{
			header.setHeader(200, this->_methods);
			sendHeaderAndBody(header, body);
		}
		else if (check == 404) //wrong path
			sendError(header, body, 404);
		else
			sendError(header, body, 500);
	}
	closedir(dir);
}

void Response::handlePost(HeaderResponse & header, BodyResponse & body, Clients* client, std::vector<char> request)
{
	std::string content_type = header.getValueHeader(client, "Content-Type");
	std::string path = client->_head.GetPath();

	if (!content_type.empty() && content_type.substr(0, 20) == " multipart/form-data")
	{
		body.findFilename(request);
		if (body.getHasFilename())
			createFileOnServer(header, body, request);
		else //try to upload an empty file
		{
			body._body = "<html><body><h1>Empty Upload</h1></body></html>";
			header._body_len = body._body.size();
			header._content_length = header.setContentLength();
			header.setHeader(200, this->_methods);
			sendHeaderAndBody(header, body);
		}
	}
	else if (!request.empty())
	{
		std::string temp(request.begin(), request.end());
		if (temp.empty())
		{
			header.setHeader(204, this->_methods); //200 or 204?
			header.sendHeader(this->_to_close);
		}
		else
		{
			body._body = temp;
			header._body_len = temp.size(); //+2?
			header.setHeader(200, this->_methods);
			sendHeaderAndBody(header, body);
		}
	}
	else
		sendError(header, body, 500);
}

void Response::handleDelete(HeaderResponse & header, BodyResponse & body, std::string & path)
{
	if (access(path.c_str(), F_OK) != 0) //file not found
	{
		header.setHeader(404, this->_methods);
		header._content_length = "Content-Length: 16\r\n";
		body._body = "File not found";
		sendHeaderAndBody(header, body);
	}
	else
	{
		if (path.substr(0, 8) != "uploads/")
		{
			header.setHeader(403, this->_methods);
			header.sendHeader(this->_to_close);
		}
		else
		{
			if (unlink(path.c_str()) == 0) //delete file
			{
				header.setHeader(204, this->_methods);
				header.sendHeader(this->_to_close);
			}
			else //could not delete file
				sendError(header, body, 404);
		}
	}
}

void Response::handlePathDir(HeaderResponse & header, BodyResponse & body, std::string & path)
{
	int check;
	std::string index = getIndex();
	std::cout << "handlepathdir" << std::endl;
	if (index.empty())
	{
		bool autoindex = getAutoindex();
		if (autoindex)
			displayAutoindex(header, body, path);
		else
			sendError(header, body, 404);
	}
	else
	{
		std::cout << "root : " << this->_root << std::endl;
		if (path == this->_root + "/")
		{
			path += index;
			std::cout << "path handlepathdir : " << path << std::endl;
			check = body.checkBody(path.c_str());
			if (check == 0)
			{
				header.setHeader(200, this->_methods);
				sendHeaderAndBody(header, body);
			}
			else
				sendError(header, body, 404);
		}
		else
		{
			std::cout << "path handlepathdir : " << path << std::endl;
			check = body.checkBody(path.c_str());
			if (check == 0)
			{
				displayAutoindex(header, body, path);
			}
			else
				sendError(header, body, 404);
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
	header.sendHeader(this->_to_close);
	body.sendBody(header);
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
