<?php

$accept				= $_SERVER['Accept'];
$content_length		= $_SERVER['Content-Length'];
$content_type		= $_SERVER['Content-Type'];
$host				= $_SERVER['Host'];
$method				= $_SERVER['Method'];
$path				= $_SERVER['Path'];
$transfer_encoding	= $_SERVER['Transfer-Encoding'];
$user_agent			= $_SERVER['User-Agent'];
$version			= $_SERVER['Version'];
$filename           = $_SERVER['Filename'];

echo "Accept = "            . $accept               . "\n";
echo "Content-Length = "    . $content_length       . "\n";
echo "Content-Type = "      . $content_type         . "\n";
echo "Host = "              . $host                 . "\n";
echo "Method = "            . $method               . "\n";
echo "Path = "              . $path                 . "\n";
echo "Transfer-Encoding = " . $transfer_encoding    . "\n";
echo "User-Agent = "        . $user_agent           . "\n";
echo "Version = "           . $version              . "\n";
echo "Filename = "          . $filename              . "\n";

// Open the file
$file = fopen($filename, "r");

// Read the file and print it to the terminal
while (!feof($file)) {
    echo fgets($file);
}

// Close the file
fclose($file);

?>
