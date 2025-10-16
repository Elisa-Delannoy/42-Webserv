#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Script CGI Python — méthode GET

import cgi
import html

print("Content-Type: text/html; charset=utf-8\r\n\r\n")
print()  # ligne vide obligatoire entre headers et contenu

# Récupérer les paramètres GET
form = cgi.FieldStorage()
nom = form.getfirst("nom", "")
message = form.getfirst("message", "")

# Échapper le HTML pour éviter l’injection
nom = html.escape(nom)
message = html.escape(message)

# Générer la page HTML
print(f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Résultat du formulaire</title>
</head>
<body>
    <h1>Données reçues</h1>
    <p><strong>Nom :</strong> {nom}</p>
    <p><strong>Message :</strong><br> {message}</p>
</body>
</html>""")
