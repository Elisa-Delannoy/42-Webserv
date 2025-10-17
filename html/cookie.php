<?php

if ($_SERVER["REQUEST_METHOD"] === "POST" && isset($_POST["username"]) && isset($_POST["Value"])) {
    $username = $_POST["username"];
    $Value = $_POST["Value"];	
    
    setcookie($username, $Value, time() + 3600, "/");
    $_COOKIE[$username] = $Value;
}

if(isset($_COOKIE["username"])) {
    echo "Bonjour " . htmlspecialchars($_COOKIE["username"]);
} else {
    echo "Cookie non trouvé";
}
?>
