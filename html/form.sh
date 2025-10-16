#!/bin/bash
echo "Content-type: text/html"
echo ""

METHOD=$REQUEST_METHOD

if [ "$METHOD" = "POST" ]; then
  read -r POST_DATA
else
  POST_DATA="$QUERY_STRING"
fi

NOM=$(echo "$POST_DATA" | sed -n 's/.*nom=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')
MESSAGE=$(echo "$POST_DATA" | sed -n 's/.*message=\([^&]*\).*/\1/p' | sed 's/+/ /g' | sed 's/%20/ /g')

urldecode() {
  echo -e "$(sed 's/+/ /g; s/%/\\x/g')"
}

NOM=$(echo "$NOM" | urldecode)
MESSAGE=$(echo "$MESSAGE" | urldecode)

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
