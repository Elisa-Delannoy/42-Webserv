#!/usr/bin/python3
# -*- coding: utf-8 -*-

import cgi
import html
import sys

# En-tête CGI obligatoire
print("Content-Type: text/html\r\n\r\n")

# Récupérer les données POST (ou GET)
form = cgi.FieldStorage()

# Récupération des champs
nom = html.escape(form.getfirst("nom", ""))
message = html.escape(form.getfirst("message", ""))

# Conversion des retours à la ligne
message = message.replace("\n", "<br>")

# Affichage HTML
print("""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Résultat du formulaire</title>
</head>
<body>
    <h1>Données reçues</h1>
    <p><strong>Nom :</strong> {}</p>
    <p><strong>Message :</strong><br> {}</p>
</body>
</html>
""".format(nom, message))