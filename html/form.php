#!/usr/bin/php-cgi
<?php

parse_str(file_get_contents("php://input"), $_POST);

$nom = isset($_POST['nom']) ? htmlspecialchars($_POST['nom']) : '';
$email = isset($_POST['email']) ? htmlspecialchars($_POST['email']) : '';
$message = isset($_POST['message']) ? nl2br(htmlspecialchars($_POST['message'])) : '';

?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <title>Résultat du formulaire</title>
</head>
<body>
    <h1>Données reçues</h1>
    <p><strong>Nom :</strong> <?= $nom ?></p>
    <p><strong>Email :</strong> <?= $email ?></p>
    <p><strong>Message :</strong><br> <?= $message ?></p>
</body>
</html>