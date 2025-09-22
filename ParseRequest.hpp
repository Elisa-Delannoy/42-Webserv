/*

2. Analyser la ligne de requête (Request line)
La première ligne contient généralement trois éléments :
Méthode HTTP (ex : GET, POST, PUT, DELETE)
Chemin ou URI (ex : /index.html, /api/users?id=5)
Version du protocole HTTP (ex : HTTP/1.1)
Il faut découper cette ligne par espaces pour extraire ces trois éléments.

http://example.com:8080/path/to/resource?param1=value1&param2=value2#fragment
Voici la décomposition de chaque partie :

Protocole (http:// ou https://) : Spécifie le protocole de communication entre le client et le serveur. Le https est sécurisé (crypté) par rapport à http.
Nom de domaine ou adresse IP (example.com) : Le nom de domaine du serveur (ou son adresse IP).
Port (:8080) : Le numéro de port sur lequel le serveur écoute. Le port par défaut pour HTTP est 80 et pour HTTPS, c'est 443, donc il est souvent omis.
Chemin (/path/to/resource) : Indique la ressource spécifique demandée (par exemple, un utilisateur particulier ou une liste d'articles).
Paramètres de la requête (?param1=value1&param2=value2) : Une série de paramètres clé-valeur passés à l'URL. Ils permettent de filtrer ou de personnaliser la demande.
Fragment (#fragment) : Utilisé pour indiquer une section spécifique d'une page web, souvent ignoré par les serveurs dans les requêtes HTTP.

Paramètres de requête dans l'URL :
Ce sont des paires clé-valeur ajoutées à l'URL après le ?.
Les paramètres sont séparés par des & et les clés et valeurs sont séparées par des =.

3. Extraire les headers (entêtes)
Après la ligne de requête, on a une série de lignes sous la forme Clé: Valeur
Chaque header fournit des infos importantes (ex : Host, User-Agent, Content-Type, Content-Length, etc.)
Il faut lire ces lignes jusqu’à rencontrer une ligne vide (indiquant la fin des headers).
Ces headers sont souvent stockés dans une structure clé-valeur (comme un dictionnaire).

4. Analyser le corps (body)
Si la requête contient un corps (ex : dans les requêtes POST, PUT), il vient après la ligne vide qui suit les headers.
La taille du corps est souvent donnée par le header Content-Length.
Le corps peut contenir différentes données (JSON, formulaire URL-encodé, fichier, etc.) selon le Content-Type.
Il faut stocker ce corps en fonction de son type pour le traiter correctement.

Que faire concrètement avec le corps d’une requête ?
1. Lire le corps depuis la requête
D’abord, tu dois extraire le corps brut (la chaîne ou données binaires) après avoir lu la taille (Content-Length) ou en gérant un transfert chunked.
Sans lire correctement le corps, tu ne peux rien en faire.

2. Identifier son format avec Content-Type
Regarde le header Content-Type pour savoir comment interpréter ce corps (JSON, formulaire, XML, multipart, etc.).
S’il n’y a pas de corps (ex: requête GET), il n’y aura pas de Content-Type.

3. Parser le corps
Selon le format, applique un parseur adapté :
JSON → parser JSON (avec une lib ou à la main)
Formulaire URL-encoded → découpe clé=valeur
Multipart → découpe en parties (pour fichiers)
Texte brut → utilise tel quel ou convertis
XML → parse XML

4. Utiliser les données extraites
Après parsing, tu as des données structurées (variables, objets, fichiers, etc.).
Tu peux :
Valider ces données (ex: champs obligatoires, formats)
Stocker en base de données
Effectuer une action métier (login, ajout de produit, etc.)

Générer une réponse adaptée


🧠 En résumé
Voici une checklist intelligente pour gérer n'importe quel corps HTTP :
✅ Est-ce que Content-Length ou Transfer-Encoding est défini ? → sinon pas de corps.
✅ Lire les bons octets du corps (pas plus, pas moins).
✅ Lire Content-Type pour identifier le format.
✅ Gérer les cas spécifiques :

gzip ? → décompresser
chunked ? → assembler les morceaux

✅ Appliquer un parseur adapté :
json, xml, form, multipart, text, etc.

🛑 Si Content-Type inconnu → soit :
le rejeter,
soit le traiter comme application/octet-stream (fichier brut)

5. Analyser les paramètres
Il peut y avoir des paramètres dans l’URL (ex : /api/users?id=5&name=Jean)
Il faut les extraire en découpant la partie après ? et séparer chaque paramètre clé=valeur.
Il peut aussi y avoir des paramètres dans le corps (ex : formulaire POST).
*/

#ifndef PARSEREQUEST_HPP
# define PARSEREQUEST_HPP

# include "HTTPServer.hpp"
# include <map>

class ParseRequest
{
	private:
		// char*		 						_request;
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_header;
	
	public:
		ParseRequest();
		// ParseRequest(char* req);
		~ParseRequest();
		void	DivideRequest(char *buf);
		int 	DivideFirstLine(std::string& first_line);
		void 	DivideHeader(std::string& first_line);
		const std::string&	GetMethod() const;
		const std::string&	GetPath() const;
		const std::string&	GetVersion() const;
		const std::map<std::string, std::string>&	GetHeader() const;
};

#endif
