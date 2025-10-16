#!/usr/bin/php-cgi
<?php
// En-tête CGI obligatoire

// Lire les données POST
parse_str(file_get_contents("php://input"), $_POST);

// Récupérer les champs du formulaire
$nom = isset($_POST['nom']) ? htmlspecialchars($_POST['nom']) : '';
$email = isset($_POST['email']) ? htmlspecialchars($_POST['email']) : '';
$message = isset($_POST['message']) ? nl2br(htmlspecialchars($_POST['message'])) : '';
// Affichage HTML
while(1)
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