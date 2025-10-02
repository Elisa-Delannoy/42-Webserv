#!/usr/bin/env python3
# Assure-toi que ce fichier est exécutable : chmod +x test.py
# Place-le dans cgi-bin/ ou configure ton serveur pour l'exécuter en tant que CGI.

import os
import sys
import cgi
import html

def print_header():
    # En-tête CGI
    print("Content-Type: text/html; charset=UTF-8")
    print()  # ligne vide requise après les en-têtes

def html_open(title="Test CGI Python"):
    print("<!DOCTYPE html>")
    print("<html>")
    print("<head>")
    print(f"  <meta charset='utf-8'>")
    print(f"  <title>{html.escape(title)}</title>")
    print("</head>")
    print("<body>")
    print(f"<h1>{html.escape(title)}</h1>")

def html_close():
    print("</body>")
    print("</html>")

def show_env():
    print("<h2>Variables d'environnement</h2>")
    print("<pre>")
    # tri pour sortie stable
    for key in sorted(os.environ.keys()):
        value = os.environ.get(key, "")
        print(html.escape(f"{key} = {value}"))
    print("</pre>")

def show_get_post():
    # cgi.FieldStorage lit QUERY_STRING et le corps POST selon CONTENT_TYPE
    form = cgi.FieldStorage()
    # Séparer variables GET et POST : cgi.FieldStorage ne distingue pas facilement
    # On peut lire QUERY_STRING pour GET, et FieldStorage pour POST/GET global.
    qs = os.environ.get("QUERY_STRING", "")
    if qs:
        print("<h2>Données GET</h2>")
        print("<pre>")
        # Décomposer la query string de façon lisible
        for pair in qs.split("&"):
            if not pair:
                continue
            print(html.escape(pair))
        print("</pre>")

    # Afficher tous les champs lus par FieldStorage (GET+POST), mais marquer les valeurs vides
    # Si aucun champ, on indique que rien n'est présent.
    if len(form.keys()) > 0:
        print("<h2>Données POST (ou champs)</h2>")
        print("<pre>")
        for key in form.keys():
            field = form[key]
            # Un champ peut être une liste (multiple) ou un FieldStorage unique
            if isinstance(field, list):
                for idx, f in enumerate(field):
                    if f.filename:
                        # upload de fichier
                        print(html.escape(f"{key}[{idx}] = <uploaded file: {f.filename}>"))
                    else:
                        print(html.escape(f"{key}[{idx}] = {f.value}"))
            else:
                if field.filename:
                    print(html.escape(f"{key} = <uploaded file: {field.filename}>"))
                else:
                    print(html.escape(f"{key} = {field.value}"))
        print("</pre>")
    else:
        print("<h2>Aucun champ POST/GET lisible par cgi.FieldStorage</h2>")

def main():
    try:
        print_header()
        html_open("Test CGI avec Python")
        show_env()
        show_get_post()
        html_close()
    except Exception as e:
        # Erreur côté serveur : renvoyer une page minimale
        # (éviter d'émettre d'en-têtes en double)
        sys.stdout.flush()
        print("Content-Type: text/plain; charset=UTF-8")
        print()
        print("Erreur lors de l'exécution du script CGI:")
        print(str(e))

if __name__ == "__main__":
    main()
