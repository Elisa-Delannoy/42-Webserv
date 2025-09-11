/*
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
*/

#ifndef PARSEREQUEST_HPP
# define PARSEREQUEST_HPP


class ParseRequest
{
	private:
		ParseRequest();
		~ParseRequest();

	public:
};

#endif
