<?php

$accept				= $_SERVER['Accept'];
$content_type		= $_SERVER['Content-Type'];
$host				= $_SERVER['Host'];
$method				= $_SERVER['Method'];
$path				= $_SERVER['Path'];
$transfer_encoding	= $_SERVER['Transfer-Encoding'];
$user_agent			= $_SERVER['User-Agent'];
$version			= $_SERVER['Version'];
$body_path          = $_SERVER['Body-Path'];

// Open the file
$file = fopen($body_path, "r");

// Read the file and print it to the terminal
while (!feof($file)) {
    echo fgets($file);
}

// Close the file
fclose($file);

echo "Accept = "            . $accept               . "\n";
echo "Content-Type = "      . $content_type         . "\n";
echo "Host = "              . $host                 . "\n";
echo "Method = "            . $method               . "\n";
echo "Path = "              . $path                 . "\n";
echo "Transfer-Encoding = " . $transfer_encoding    . "\n";
echo "User-Agent = "        . $user_agent           . "\n";
echo "Version = "           . $version              . "\n";
echo "bodyPath = "          . $body_path            . "\n";

flush();

?>
