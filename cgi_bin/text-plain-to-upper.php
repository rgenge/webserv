<?php

if (isset($_SERVER["Body-Path"]) && $_SERVER["Body-Path"] != "") {
    if (isset($_SERVER["Content-Type"]) && $_SERVER["Content-Type"] == "text/plain") {
        $file = fopen($_SERVER["Body-Path"], "r");    
        $text = strtoupper(fread($file, filesize($_SERVER["Body-Path"])));
        fclose($file);
    }
    else
        $text = "";
}

$htmlContent = "<!DOCTYPE html>\r\n";
$htmlContent .= "<html>\r\n";
$htmlContent .= "<head>\r\n";
$htmlContent .= "    <title>200 OK</title>\r\n";
$htmlContent .= "</head>\r\n";
$htmlContent .= "<body>\r\n";
$htmlContent .= "    <h1>Successful request</h1>\r\n";
$htmlContent .= "    <p>To uppercase text: $text</p>\r\n";
$htmlContent .= "</body>\r\n";
$htmlContent .= "</html>\r\n";

$contentLength = strlen($htmlContent);

$response = "HTTP/1.1 200 OK\r\n";
$response .= "Content-Type: text/html; charset=UTF-8\r\n";
$response .= "Content-Length: $contentLength\r\n";
$response .= "\r\n";
$response .= $htmlContent;

echo $response;

?>
