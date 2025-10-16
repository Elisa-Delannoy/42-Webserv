<?php

// Vérifier si des données ont été envoyées en POST
if ($_SERVER["REQUEST_METHOD"] === "POST" && isset($_POST["username"]) && isset($_POST["Value"])) {
    $username = $_POST["username"];
    $Value = $_POST["Value"];	
    
    // Définir le cookie avec le nom envoyé
    setcookie($username, $Value, time() + 3600, "/"); // expire dans 1 heure
    $_COOKIE[$username] = $Value; // Pour pouvoir l'utiliser immédiatement
}

// Lire le cookie et afficher
if(isset($_COOKIE["username"])) {
    echo "Bonjour " . htmlspecialchars($_COOKIE["username"]);
} else {
    echo "Cookie non trouvé";
}
?>
