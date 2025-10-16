#!/bin/bash

# Indique au serveur qu'on renvoie du HTML
echo "Content-type: text/html"
echo ""

# Lire la méthode HTTP (GET ou POST)
METHOD=$REQUEST_METHOD

# Si la méthode est POST, lire le body
if [ "$METHOD" = "POST" ]; then
  read -r POST_DATA
else
  POST_DATA="$QUERY_STRING"
fi

# Extraire les champs du formulaire (nom=...&message=...)
# On découpe la chaîne pour récupérer chaque variable
NOM=$(echo "$POST_DATA" | sed -n 's/.*nom=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')
MESSAGE=$(echo "$POST_DATA" | sed -n 's/.*message=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')

# Décodage URL (fonction simple)
urldecode() {
  echo -e "$(sed 's/+/ /g; s/%/\\x/g')"
}

NOM=$(echo "$NOM" | urldecode)
MESSAGE=$(echo "$MESSAGE" | urldecode)

# Générer le HTML de réponse
cat <<EOF
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Réponse CGI Bash</title>
</head>
<body style="font-family: Arial, sans-serif; padding: 20px;">
  <h1>Réponse du script CGI en Bash 🎉</h1>
  <p><strong>Nom :</strong> $NOM</p>
  <p><strong>Message :</strong> $MESSAGE</p>
  <hr>
  <p>Merci d'avoir utilisé le script <code>test.sh</code> !</p>
</body>
</html>
EOF
