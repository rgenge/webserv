<?php

$body_path = $_SERVER['Body-Path'];

// Abre o arquivo
$file = fopen($body_path, "r");

// Lê o primeiro caractere do arquivo
$option = fgetc($file);

$text = strtoupper(fread($file, filesize($body_path)));

// Fecha o arquivo
fclose($file);

echo "HTTP/1.1 200 OK\r\n";
echo "Status: 200 OK\r\n";
echo "Content-Type: text/html\r\n";
echo "Content-Length: " . strlen("<!DOCTYPE html><html>...</html>") . "\r\n\r\n";

echo "<!DOCTYPE html>
<html>
<head>
    <title>Sucesso</title>
</head>
<body>
    <h1> testando </h1>
</body>
</html>";

?>
