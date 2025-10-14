#!/usr/bin/bash

# En-tête CGI obligatoire
echo "Content-Type: text/html"
echo ""

# Lire le corps de la requête POST
POST_DATA=""
if [ -n "$CONTENT_LENGTH" ] && [ "$CONTENT_LENGTH" -gt 0 ]; then
    POST_DATA=$(head -c "$CONTENT_LENGTH")
fi

# Extraire les champs depuis POST_DATA (nom, email, message)
nom=$(echo "$POST_DATA" | sed -n 's/.*nom=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')
email=$(echo "$POST_DATA" | sed -n 's/.*email=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')
message=$(echo "$POST_DATA" | sed -n 's/.*message=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')

# Décodage basique des caractères encodés (%xx)
urldecode() {
    local data=${1//+/ }
    printf '%b' "${data//%/\\x}"
}

nom=$(urldecode "$nom")
email=$(urldecode "$email")
message=$(urldecode "$message")

# Affichage HTML
cat <<EOF
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Résultat du formulaire</title>
</head>
<body>
    <h1>Données reçues</h1>
    <p><strong>Nom :</strong> ${nom}</p>
    <p><strong>Email :</strong> ${email}</p>
    <p><strong>Message :</strong><br> ${message}</p>
</body>
</html>
EOF
