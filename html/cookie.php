#!/usr/bin/cgi-php
<?php
// Récupère les paramètres GET
$name  = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : '';
$value = isset($_GET['value']) ? htmlspecialchars($_GET['value']) : '';

// On indique qu'on renvoie du HTML
header("Content-Type: text/html; charset=UTF-8");

// Si on a bien un nom et une valeur, on crée le cookie
if ($name && $value) {
    setcookie($name, $value, time() + 3600, "/"); // cookie valable 1h
    $message = "✅ Cookie <strong>$name</strong> créé avec la valeur <strong>$value</strong> (valable 1h)";
} else {
    $message = "❌ Aucun cookie créé — utilisez ?name=nom&value=valeur dans l’URL";
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Set Cookie (GET)</title>
</head>
<body>
    <h1><?= $message ?></h1>
    <h2>Cookies existants :</h2>
    <ul>
        <?php
        if (!empty($_COOKIE)) {
            foreach ($_COOKIE as $k => $v) {
                echo "<li><strong>$k</strong> = $v</li>";
            }
        } else {
            echo "<li>Aucun cookie trouvé.</li>";
        }
        ?>
    </ul>
</body>
</html>
