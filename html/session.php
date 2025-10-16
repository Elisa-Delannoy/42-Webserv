#!/usr/bin/php-cgi
<?php
// Démarre la session ou récupère celle existante
session_start();

// Vérifie si une session existe déjà pour cet utilisateur
if (!isset($_SESSION['id'])) {
    // Crée un identifiant unique pour cette session
    $_SESSION['id'] = uniqid();  // ou tu peux mettre un int aléatoire
}

// Tu peux maintenant stocker d'autres données
$_SESSION['background_color'] = '#f1cb83';
$_SESSION['username'] = 'Noah';

echo "Session créée ! ID : " . $_SESSION['id'];
?>
