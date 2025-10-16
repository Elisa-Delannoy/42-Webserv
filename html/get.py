#!/usr/bin/python3
# -*- coding: utf-8 -*-

import cgi
import html

print("Content-Type: text/html; charset=utf-8")
print()

form = cgi.FieldStorage()
nom = form.getfirst("nom", "")
message = form.getfirst("message", "")

nom = html.escape(nom)
message = html.escape(message)

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
