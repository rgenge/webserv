<?php

$method = $_SERVER['METHOD'];
$userAgent = $_SERVER['HTTP_USER_AGENT'];
$remoteAddr = $_SERVER['REMOTE_ADDR'];
$customVar1 = $_SERVER['CUSTOM_VAR1'];
$customVar2 = $_SERVER['CUSTOM_VAR2'];

echo "METHOD: " . $method . "<br>";
echo "HTTP_USER_AGENT: " . $userAgent . "<br>";
echo "REMOTE_ADDR: " . $remoteAddr . "<br>";
echo "CUSTOM_VAR1: " . $customVar1 . "<br>";
echo "CUSTOM_VAR2: " . $customVar2 . "<br>";
?>
