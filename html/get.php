#!/usr/bin/php-cgi
<?php
// Script CGI PHP — méthode GET

// Indiquer au serveur qu'on renvoie du HTML
header("Content-Type: text/html; charset=utf-8");

// Récupérer les paramètres GET
$nom = isset($_GET['nom']) ? htmlspecialchars($_GET['nom']) : '';
$message = isset($_GET['message']) ? htmlspecialchars($_GET['message']) : '';

// Générer la page HTML
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <title>Résultat du formulaire</title>
</head>
<body>
    <h1>Données reçues</h1>
    <p><strong>Nom :</strong> <?= $nom ?></p>
    <p><strong>Message :</strong><br> <?= $message ?></p>
</body>
</html>