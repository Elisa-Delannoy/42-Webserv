#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html; charset=utf-8");

$nom = isset($_GET['nom']) ? htmlspecialchars($_GET['nom']) : '';
$message = isset($_GET['message']) ? htmlspecialchars($_GET['message']) : '';

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