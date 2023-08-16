<?php

$body_path = $_SERVER['Body-Path'];

// Abre o arquivo
$file = fopen($body_path, "r");

// Lê o primeiro caractere do arquivo
$option = fgetc($file);

if ($option == 'A') {
    $text = strtoupper(fread($file, filesize($body_path)));
}
else {
    $text = strtolower(fread($file, filesize($body_path)));
}

// Fecha o arquivo
fclose($file);

echo $text;

?>
