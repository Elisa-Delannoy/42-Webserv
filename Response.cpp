#include "Response.hpp"

Response::Response(std::map<int, std::string> errors_path, int client_fd, int body_len) :
	_errors_path(errors_path), _client_fd(client_fd), _body_len(body_len)
{ }

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


ETAPES A SUIVRE :
1. verifier si le body est bon et pret a envoyer (si body il y a)
2. envoyer header adequat
3. envoyer body. Si erreur avec send => on stop send et osef

*/
void Response::setHeader(std::string version, std::string path, int code)
{
	if (code == 200)
	{
		this->_status = setStatus(version, " 200 OK\r\n");
		this->_content_length = setContentLength(path);
	}
	if (code == 204)
	{
		this->_status = setStatus(version, " 204 No Content\r\n");
		this->_content_length = "0";
		return ;
	}
	if (code == 404)
	{
		this->_status = setStatus(version, " 404 Not Found\r\n");
		this->_content_length = "0";
		return ;
	}
	if (code == 500)
	{
		this->_status = setStatus(version, " 500 Internal Server Error\r\n");
		this->_content_length = "0";
	}
	this->_content_type = setContentType(path);
}

void Response::sendHeader()
{
	this->_response = this->_status + this->_content_type
		+ this->_content_length + "\r\n";

	if(send(this->_client_fd, this->_response.c_str(), this->_response.size(), 0) == -1)
		std::cerr << "Error while sending headers." << std::endl;
}

void Response::sendHeaderAndBody()
{
	sendHeader();
	sendBody();
}

void Response::sendError(int code)
{
	if (this->_errors_path.empty())
	{
		std::cout << "---------EMPTY---------" << std::endl;
		if (code == 404)
			this->_content = ERROR404;
		if (code == 500)
			this->_content = ERROR500;
		this->_content_length = setContentLength(path);
	}
	else
	{
		std::cout << "---------FILLED---------" << std::endl;
		std::string path = GetErrorPath(code).c_str();
		checkBody(path.c_str()+1);
		this->_content_length = setContentLength(path);
	}
	sendHeaderAndBody();
}

void Response::sendResponse(ParseRequest header, char* buf)
{
	(void)buf;

	std::string path = header.GetPath();
	std::string method = header.GetMethod();
	std::string version = header.GetVersion();

	if (method == "GET")
	{
		if (path == "/")
		{
			if (checkBody(ROOT) == 0)
			{
				setHeader(version, path, 200);
				sendHeaderAndBody();
			}
			else
			{
				setHeader(version, path, 500);
				sendError(500);
			}
		}
		else if (path.substr(0, 5) == "/img/")
		{
			if (checkBody(path.substr(1).c_str()) == 0) //path without first '/'
			{
				setHeader(version, path, 200);
				sendHeaderAndBody();
			}
			else if (checkBody(path.substr(1).c_str()) == 404) //wrong path
			{
				setHeader(version, path, 404);
				sendHeader();
			}
			else
			{
				setHeader(version, path, 500);
				sendHeader();
			}
		}
		else if (path == "/favicon.ico") //just in case, we ignore it
		{
			setHeader(version, path, 204);
			sendHeader();
		}
	}
	else if (method == "POST")
	{

	}
}

void Response::sendBody()
{
	size_t data_sent = 0;
	while(data_sent < this->_content.size())
	{
		ssize_t data_read = send(this->_client_fd, this->_content.data() + data_sent,
			this->_content.size() - data_sent, 0);
		if (data_read == -1)
		{
			std::cerr << "Error while sending content." << std::endl;
			break;
		}
		data_sent += data_read;
	}
}

//Return 0 if ok
//Return 1 if not
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

std::string Response::setStatus(std::string version, std::string code)
{
	return (version + code);
}

std::string Response::setContentType(std::string path)
{
	std::string ret = "Content-Type: ";
	if (path == "/")
	{
		ret += "text/html";
	}
	else
	{
		size_t i = path.size() - 1;
		while (path[i - 1] != '.')
			i--;
		std::string type = path.substr(i);

		if (type == "jpg")
			ret += "image/jpg";
		if (type == "jpeg")
			ret += "image/jpeg";
		if (type == "png")
			ret += "image/png";
		if (type == "gif")
			ret += "image/gif";
		if (type == "svg")
			ret += "image/svg+xml";
		if (type == "webp")
			ret += "image/webp";
		if (type == "ico")
			ret += "image/x-icon";
		if (type == "avif")
			ret += "image/avif";
	}
	return (ret + "\r\n");
}

std::string Response::setSize(const char* path_image)
{
	std::ostringstream oss;
	if (stat(path_image, &this->_info) < 0)
	{
		std::cerr << "Error stat file" << std::endl;
	}
	oss << this->_info.st_size;
	this->_body_len = this->_info.st_size;
	return oss.str();
}

std::string Response::setContentLength(std::string path)
{
	std::ostringstream oss;
	std::string size;
	if (this->_body_len > 0)
	{
		oss << this->_body_len;
		size = oss.str();
	}
	else if (path == "/")
	{
		size = setSize(ROOT);
	}
	else
	{
		size = setSize(path.c_str() + 1);
	}
	return "Content-Length: " + size + "\r\n";
}


std::string Response::GetErrorPath(int code)
{
	return (this->_errors_path[code]);
}

/* void Response::sendBody(ParseRequest request, char* buf)
{
	else if (path == "/upload")
	{
		std::string boundary;
		for(int i = 0; buf[i] != '\r'; i++)
			boundary += buf[i];

		int i = 0;
		for(; i < this->_body_len; i++)
		{
			if (buf[i] == 'R' && buf[i+1] == 'I' && buf[i+2] == 'F' && buf[i+3] == 'F')
				break;
		}

		std::cout << std::endl;
		std::cout << "BODY : " << buf+i << std::endl;

		int j = i;
		for(; j < this->_body_len; j++)
		{
			if (buf[j] == '-' && buf[j + 1] == '-' && buf[j + 2] == '-'
				&& buf[j+3] == '-' && buf[j + 4] == '-' && buf[j + 5] == '-'
				&& buf[j+6] == 'g')
				break;
		}
		int endfile = j;
		if (buf[j-2] == '\r' && buf[j-1] == '\n')
			endfile = j - 2;
	
		// std::ofstream out("uploads/fichier.png", std::ios::binary);
		// out.write(buf + i, endfile - i);
		// out.close();
	}
} */
