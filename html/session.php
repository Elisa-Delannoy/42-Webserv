#!/usr/bin/php-cgi
<?php
session_start();

if (!isset($_SESSION['id'])) {
    $_SESSION['id'] = uniqid();
}

$_SESSION['background_color'] = '#f1cb83';
$_SESSION['username'] = 'Noah';

echo "Session créée ! ID : " . $_SESSION['id'];
?>
