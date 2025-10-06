#!/usr/bin/node
// Assure-toi que ce fichier est exécutable : chmod +x test.js
// À utiliser en CGI : place dans cgi-bin/ ou dans un serveur configuré pour Node CGI.

const querystring = require('querystring');

// Lire les données POST
function readPostData(callback) {
  let input = '';
  process.stdin.setEncoding('utf8');
  process.stdin.on('data', chunk => { input += chunk });
  process.stdin.on('end', () => {
    callback(querystring.parse(input));
  });
}

// Échapper le HTML
function h(str) {
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

// Génère la page HTML avec GET + POST + env
function renderPage(postData = {}) {
  const env = process.env;
  const query = querystring.parse(env.QUERY_STRING || '');

  // En-tête CGI obligatoire
  console.log("Content-Type: text/html; charset=UTF-8\n");

  console.log(`<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Test CGI Node.js</title>
</head>
<body>
  <h1>Test CGI avec Node.js</h1>

  <h2>Variables d'environnement</h2>
  <pre>`);
  Object.keys(env).sort().forEach(key => {
    console.log(`${h(key)} = ${h(env[key])}`);
  });
  console.log(`</pre>`);

  console.log(`<h2>Données GET</h2>
  <pre>`);
  if (Object.keys(query).length === 0) {
    console.log("Aucune donnée GET");
  } else {
    Object.entries(query).forEach(([k, v]) => {
      console.log(`${h(k)} = ${h(v)}`);
    });
  }
  console.log(`</pre>`);

  console.log(`<h2>Données POST</h2>
  <pre>`);
  if (Object.keys(postData).length === 0) {
    console.log("Aucune donnée POST");
  } else {
    Object.entries(postData).forEach(([k, v]) => {
      console.log(`${h(k)} = ${h(v)}`);
    });
  }
  console.log(`</pre>`);

  console.log(`<hr>
<h2>Formulaire de test (POST)</h2>
<form method="post" action="">
  <label>champ1: <input type="text" name="champ1" value="val1"></label><br>
  <label>champ2: <input type="text" name="champ2" value="val2"></label><br>
  <input type="submit" value="Envoyer (POST)">
</form>
</body>
</html>`);
}

// Lire POST, puis afficher la page
if (process.env.REQUEST_METHOD === 'POST') {
  readPostData(renderPage);
} else {
  renderPage();
}