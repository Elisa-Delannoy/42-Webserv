#include "Response.hpp"

Response::Response(int client_fd, int body_len) : _client_fd(client_fd), _body_len(body_len)
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

std::string Response::setStatus(std::string version)
{
	return (version + " 200 OK\r\n");
}

std::string Response::setContentType(std::string path)
{
	std::string ret = "Content-Type: ";
	std::string type;
	if (path == "/")
	{
		ret += "text/html";
	}
	else if (path == "/favicon.ico")
	{
		ret += "image/svg+xml";
	}
	else if (path == "/upload")
	{
		ret += "image/webp";
	}
	else
	{
		ret += "image/jpeg";
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
		size = setSize("html/index.html");
	}
	else if (path == "/favicon.ico")
	{
		size = setSize("img/favicon.svg");
	}
	else
	{
		size = setSize("img/cookie.jpeg");
	}
	return "Content-Length: " + size + "\r\n";
}

void Response::sendHeaders(ParseRequest header)
{
	this->_status = setStatus(header.GetVersion());
	this->_content_type = setContentType(header.GetPath());
	this->_content_length = setContentLength(header.GetPath());

	this->_response = this->_status + this->_content_type + this->_content_length + "\r\n";

	if(send(this->_client_fd, this->_response.c_str(), this->_response.size(), 0) == -1)
		std::cerr << "Error while sending headers." << std::endl;
}

void Response::sendImage(std::string path_image)
{
	std::ifstream ifs(path_image.c_str() + 1, std::ios::binary); //c.str() + 1 to skip first '/'
	char *buffer = new char[this->_body_len];
	ifs.read(buffer, this->_body_len);
	int data_sent = 0;
	while(data_sent < this->_body_len)
	{
		ssize_t data_read = send(this->_client_fd, buffer + data_sent, this->_body_len - data_sent, 0);
		if (data_read == -1)
			std::cerr << "Error while sending content." << std::endl;
		data_sent += data_read;
	}
	delete[] buffer;
}

void Response::sendBody(ParseRequest request, char* buf)
{
	std::string path = request.GetPath();
	if (path == "/")
	{
		std::ifstream file("html/index.html");
		std::stringstream buffer;

		buffer << file.rdbuf();
		this->_content = buffer.str();
		if(send(this->_client_fd, this->_content.c_str(), this->_content.size(), 0) == -1)
			std::cerr << "Error while sending content." << std::endl;
	}
	else if (path == "/favicon.ico")
	{
		sendImage("/img/favicon.svg");
	}
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
	
		std::ofstream out("uploads/fichier.png", std::ios::binary);
		out.write(buf + i, endfile - i);
		out.close();
	}
	else
	{
		sendImage(path);
	}
}
