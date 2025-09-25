#!/usr/bin/php
<?php
// Assure-toi que ce fichier est exécutable : chmod +x test.php

header("Content-Type: text/html; charset=UTF-8");

echo "<!DOCTYPE html>\n<html>\n<head><title>Test CGI PHP</title></head>\n<body>\n";
echo "<h1>Test CGI avec PHP</h1>\n";

// Variables CGI (transmises par le serveur)
echo "<h2>Variables d'environnement</h2>\n<pre>";
foreach ($_SERVER as $key => $value) {
    echo htmlspecialchars("$key = $value") . "\n";
}
echo "</pre>\n";

// Données GET
if (!empty($_GET)) {
    echo "<h2>Données GET</h2>\n<pre>";
    print_r($_GET);
    echo "</pre>\n";
}

// Données POST
if (!empty($_POST)) {
    echo "<h2>Données POST</h2>\n<pre>";
    print_r($_POST);
    echo "</pre>\n";
}

echo "</body>\n</html>";
?>
