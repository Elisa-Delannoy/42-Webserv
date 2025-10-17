#!/usr/bin/python3
import cgi
import html
import sys

print("Content-Type: text/html\r\n\r\n")

form = cgi.FieldStorage()

nom = html.escape(form.getfirst("nom", ""))
message = html.escape(form.getfirst("message", ""))

message = message.replace("\n", "<br>");;

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