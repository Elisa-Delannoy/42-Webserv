#include "Response.hpp"
#include "HeaderResponse.hpp"
#include <cerrno>

Response::Response(ServerConf & servers, Clients* client) : _server(servers)
{
	this->_errors_path = this->_server.GetErrorPath();
	this->_body_len = client->_body.GetContentLen();
	this->_client_fd = client->GetSocket();
}

Response::~Response()
{ }

/*
| **Code HTTP**                                           | **Body nécessaire ?** | **Exemple concret**                                          | **Comment l’envoyer**                                                                                 |
| ------------------------------------------------------- | --------------------- | ------------------------------------------------------------ | ----------------------------------------------------------------------------------------------------- |
| `200 OK`                                                | Oui                   | `GET /` → `index.html`, image, JSON                          | Lire le fichier ou générer le body → envoyer header (`Content-Type`, `Content-Length`) → envoyer body |
| `204 No Content`                                        | Non                   | `GET /favicon.ico` quand on ne sert pas d’icône              | Envoyer seulement le header `HTTP/1.1 204 No Content`                                                 |
| `301 Moved Permanently` / `302 Found` / `303 See Other` | Non                   | Redirection après un POST ou changement d’URL                | Envoyer header avec `Location: /nouvelle-page` → body optionnel ou vide                               |
| `304 Not Modified`                                      | Non                   | Cache : le fichier n’a pas changé depuis la dernière requête | Envoyer seulement le header `HTTP/1.1 304 Not Modified`                                               |
| `404 Not Found`                                         | Optionnel             | Fichier demandé inexistant                                   | Header seul si minimaliste, ou header + body HTML si tu veux afficher un message                      |
| `500 Internal Server Error`                             | Optionnel             | Problème serveur lors de la lecture d’un fichier             | Header seul ou header + body HTML expliquant l’erreur                                                 |


Si l’upload est réussi ✅

Tu peux répondre avec 200 OK et un petit message HTML :
HTTP/1.1 200 OK
Content-Type: text/html; charset=UTF-8
Content-Length: ...

<html>
  <body>
    <h1>Upload réussi !</h1>
  </body>
</html>

Ou bien, tu peux renvoyer 201 Created si tu veux être strict avec HTTP, surtout si tu as créé une “ressource” (le fichier sur ton serveur).
Exemple :
HTTP/1.1 201 Created
Location: /uploads/monfichier.jpg


Si l’upload échoue ❌

Tu renvoies un code d’erreur approprié :

400 Bad Request si la requête est mal formée.

413 Payload Too Large si le fichier est trop gros.

500 Internal Server Error si c’est un bug serveur.




Quand le client fait un POST, il envoie des données (ex: formulaire d’upload).

Si ton serveur renvoie directement du HTML avec 200 OK, le navigateur reste sur l’URL du POST.

Si l’utilisateur rafraîchit la page, le navigateur va proposer de renvoyer le POST → ça peut provoquer un deuxième upload (pas cool).

👉 Pour éviter ça, on utilise le pattern PRG (Post / Redirect / Get) :

Le client envoie un POST avec son fichier.

Le serveur traite le fichier, l’enregistre, puis répond :
HTTP/1.1 303 See Other
Location: /upload_success.html
Le navigateur reçoit ça et fait automatiquement un GET /upload_success.html.

L’utilisateur voit une page de confirmation, et un refresh de la page ne répète pas le POST.
Après avoir écrit le fichier côté serveur, tu peux envoyer quelque chose comme :
HTTP/1.1 303 See Other
Location: /upload_success.html
Content-Length: 0
ici pas de corps (body), juste l’en-tête Location. Le navigateur ira chercher /upload_success.html en GET.

Comparaison avec les autres codes possibles

200 OK : tu affiches directement la page de confirmation → mais le problème de "rafraîchir = re-upload" existe.

201 Created : indique que la ressource est créée (bien pour une API REST), mais dans un site web classique ça oblige le client à rester sur l’URL du POST.

303 See Other : meilleure pratique pour un site avec formulaire ou upload → évite les re-POST accidentels.

*/

void Response::sendError(HeaderResponse & header, int code)
{
	if (this->_errors_path.find(code)->second.empty())
	{
		if (code == 404)
			this->_content = ERROR404;
		if (code == 500)
			this->_content = ERROR500;
		std::ostringstream oss;
		oss << this->_content.size();
		this->_content_length = "Content-Length: " + oss.str() + "\r\n";
	}
	else
	{
		std::string path = GetErrorPath(code).c_str();
		checkBody(path.c_str()+1);
		this->_content_length = header.setContentLength();
	}
	sendHeaderAndBody(header);
}

//Loop through locations from conf file to find correct root
void Response::setRootLocation(std::string & path)
{
	std::string name;

	for (int i = 0; i < this->_server._nb_location; i++)
	{
		name = this->_server.GetLocation(i).GetName();
		std::cout << "index : " <<  this->_server.GetLocation(i).GetIndex() << std::endl;
		std::cout << "location : " << name << std::endl;
		if (name == "/")
			this->_index_location = i;
		if (!name.empty() && name != "/" && path.compare(0, name.size(), name) == 0)
		{
			this->_index_location = i;
			this->_root = this->_server.GetLocation(i).GetRoot() + "/";
			this->_root.erase(this->_root.begin(), this->_root.begin()+1);
			path.replace(0, name.size(), this->_root);
			std::cout << "path : " << path << std::endl;
			return;
		}
	}
	if (this->_root.empty())
	{
		this->_root = this->_server.GetLocation(this->_index_location).GetRoot();
		this->_root.erase(this->_root.begin(), this->_root.begin()+1);
		path.replace(0, name.size() - 1, this->_root);
		std::cout << "path : " << path << std::endl;
	}
}

//Return index from conf file
std::string Response::getIndex()
{
	return this->_server.GetLocation(this->_index_location).GetIndex();
}

void Response::displayAutoindex(HeaderResponse & header, std::string path)
{
	DIR *dir;
	dir = opendir(path.c_str());
	if (dir == NULL)
	{
		std::cout << "path opendir error : " << path << std::endl;
		header.setHeader(404);
		sendError(header, 404);
	}
	else
	{
		struct dirent* dirp;
		this->_content = "<html><body><h1>AUTOINDEX</h1><ul>";
		dirp = readdir(dir);
		while (dirp != NULL)
		{
			std::string name = dirp->d_name;
			size_t found = name.find(".");
			if (found == std::string::npos)
				name += "/";
			this->_content += "<li><a href=\"" + name + "\">" + name + "</a></li>";
			dirp = readdir(dir);
		}
		this->_content += "</ul></body></html>";
		header._body_len = this->_content.size();
		header._content_length = header.setContentLength();
		header.setHeader(200);
		sendHeaderAndBody(header);
	}
}

void Response::displayUploadSuccessfull(HeaderResponse & header)
{
	this->_content = "<html><body><h1>Upload Successfull!</h1></body></html>";
	header._body_len = this->_content.size();
	header._content_length = header.setContentLength();
	header.setHeader(200);
	sendHeaderAndBody(header);
}

bool Response::getAutoindex()
{
	return this->_server.GetLocation(this->_index_location).GetAutoindex();
}

void Response::sendResponse(ServerConf & servers, Clients* client, std::vector<char> buf)
{
	(void)buf;
	std::string path = client->_head.GetPath();
	std::string method = client->_head.GetMethod();
	std::string version = client->_head.GetVersion();
	std::cout << "|" << path << "|" << method << "|" << version << "|" << std::endl;
	
/* 	std::cout << "\n\n--------BUF BEGIN--------" << std::endl;
	std::vector<char>::iterator it = buf.begin();
	for(; it != buf.end(); it++)
		std::cout << *it;
	std::cout << "\n--------BUF END-------\n" << std::endl; */

	setRootLocation(path);
	HeaderResponse header(servers, client, path, version);

	if (method == "GET")
	{
		handleGet(header, path);
	}
	else if (method == "POST")
	{
		std::cout << "ENTERING POST PROCESSING" << std::endl;
		// client->_body._multipart;
		std::string filename = "uploads/" + client->_body._multipart[0].filename;
		std::cout << "filename : " << filename << std::endl;
		std::ofstream out(filename.c_str(), std::ios::binary);

		for(size_t i = 0; i < client->_body._multipart.size(); i++)
		{
			out.write(client->_body._multipart[i].content.data(),
				client->_body._multipart[i].content.size());
		}
		out.close();
		std::cout << "upload write file done" << std::endl;
		displayUploadSuccessfull(header);
	}
}

void Response::handleGet(HeaderResponse & header, std::string & path)
{
	int check;
	if (opendir(path.c_str()) != NULL) //path is a dir
	{
		handlePathDir(header, path);
	}
	else //path is a file
	{
		std::cout << "path : " << path << std::endl;
		check = checkBody(path.c_str());
		if (check == 0)
		{
			header.setHeader(200);
			sendHeaderAndBody(header);
		}
		else if (check == 404) //wrong path
		{
			header.setHeader(404);
			header.sendHeader();
		}
		else
		{
			header.setHeader(200);
			header.sendHeader();
		}
	}
}

void Response::handlePathDir(HeaderResponse & header, std::string & path)
{
	int check;
	std::string index = getIndex();

	if (index.empty())
	{
		std::cout << "index is empty" << std::endl;

		bool autoindex = getAutoindex();
		if (autoindex)
		{
			displayAutoindex(header, path);
		}
		else
		{
			header.setHeader(404);
			sendError(header, 404);
		}
	}
	else
	{
		path += index;
		std::cout << "path : " << path << std::endl;
		check = checkBody(path.c_str());
		if (check == 0)
		{
			header.setHeader(200);
			sendHeaderAndBody(header);
		}
		else
		{
			header.setHeader(500);
			sendError(header, 500);
		}
	}
}

//---------------------------HEADER---------------------------

std::string Response::GetErrorPath(int code)
{
	return (this->_errors_path[code]);
}

//---------------------------BODY---------------------------

void Response::sendBody()
{
	size_t data_sent = 0;
	while(data_sent < this->_content.size())
	{
		ssize_t data_read = send(this->_client_fd, this->_content.data() + data_sent,
			this->_content.size() - data_sent, 0);
	// 	if (errno == EPIPE) {
    //     std::cerr << "Client closed connection (EPIPE)." << std::endl;
    // } else {
    //     std::cerr << "Send error: " << strerror(errno) << std::endl;
    // }
	// 	std::cout << "data read" << data_read << std::endl;
		if (data_read == -1)
		{
			std::cerr << "Error while sending content." << std::endl;
			break;
		}
		data_sent += data_read;
	}
}

//Return 0 if ok
//Return 1 if reading problem
//Return 404 if file problem
int Response::checkBody(const char* path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) //wrong path, invalid rights, inexisting file
		return 404;

	std::stringstream buffer;
	buffer << file.rdbuf();
	if (file.fail()) //reading problem
		return 1;

	file.close();
	this->_content = buffer.str();
	return 0;
}

void Response::sendHeaderAndBody(HeaderResponse & header)
{
	header.sendHeader();
	sendBody();
}
