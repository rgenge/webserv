<?php

if (isset($_SERVER["Body-Path"]) && $_SERVER["Body-Path"] != "") {
    $file = fopen($_SERVER["Body-Path"], "r");   
    $text = fread($file, filesize($_SERVER["Body-Path"]));
    fclose($file);
}

$name = "";
$email = "";
$nickname = "";


parse_str($text, $params);
if (isset($params["name"]) && $params["name"] != "") {
    $name =  "| Name: " . $params["name"] . " | ";
}
if (isset($params["nickname"]) && $params["nickname"] != "") {
    $nickname =  "Nickname: " . $params["nickname"] . " | ";
}
if (isset($params["email"]) && $params["email"] != "") {
    $email =  "E-mail: " . $params["email"] . " |";
}

$result = $name . $nickname . $email;

if ($name != "" && $nickname != "" && $email != "") {
    $htmlContent = "<!DOCTYPE html>\r\n";
    $htmlContent .= "<html>\r\n";
    $htmlContent .= "<head>\r\n";
    $htmlContent .= "    <title>200 OK</title>\r\n";
    $htmlContent .= "</head>\r\n";
    $htmlContent .= "<body>\r\n";
    $htmlContent .= "    <h1>Successful request</h1>\r\n";
    $htmlContent .= "    <p>$result</p>\r\n";
    $htmlContent .= "</body>\r\n";
    $htmlContent .= "</html>\r\n";
    
    $contentLength = strlen($htmlContent);
    
    $response = "HTTP/1.1 200 OK\r\n";
    $response .= "Content-Type: text/html; charset=UTF-8\r\n";
    $response .= "Content-Length: $contentLength\r\n";
    $response .= "\r\n";
    $response .= $htmlContent;
} else {
    $htmlContent .= '<!DOCTYPE html>' . "\r\n";
    $htmlContent .= '<html lang="en">' . "\r\n";
    $htmlContent .= '<head>' . "\r\n";
    $htmlContent .= '	<meta charset="UTF-8">' . "\r\n";
    $htmlContent .= '	<meta name="viewport" content="width=device-width, initial-scale=1.0">' . "\r\n";
    $htmlContent .= '	<title>Document</title>' . "\r\n";
    $htmlContent .= '	<style>' . "\r\n";
    $htmlContent .= '		.ErrorBox {' . "\r\n";
    $htmlContent .= '			width: 563px;' . "\r\n";
    $htmlContent .= '			height: 154px;' . "\r\n";
    $htmlContent .= '			background: #D9D9D9;' . "\r\n";
    $htmlContent .= '			box-shadow: 0px 4px 4px 0px rgba(0, 0, 0, 0.25);' . "\r\n";
    $htmlContent .= '			position: fixed;' . "\r\n";
    $htmlContent .= '			top: 50%;' . "\r\n";
    $htmlContent .= '			left: 50%;' . "\r\n";
    $htmlContent .= '			transform: translate(-50%, -50%);' . "\r\n";
    $htmlContent .= '			text-align: center;' . "\r\n";
    $htmlContent .= '			justify-content: center;' . "\r\n";
    $htmlContent .= '			align-items: center;' . "\r\n";
    $htmlContent .= '		}' . "\r\n";
    $htmlContent .= '		.ErrorTitle {' . "\r\n";
    $htmlContent .= '			position: fixed;' . "\r\n";
    $htmlContent .= '			width: 500px;' . "\r\n";
    $htmlContent .= '			height: 22px;' . "\r\n";
    $htmlContent .= '			flex-direction: column;' . "\r\n";
    $htmlContent .= '			flex-shrink: 0; ' . "\r\n";
    $htmlContent .= '			color: #000;' . "\r\n";
    $htmlContent .= '			font-size: 32px;' . "\r\n";
    $htmlContent .= '			font-family: Arial; ' . "\r\n";
    $htmlContent .= '			transform: translate(5%, -100%);' . "\r\n";
    $htmlContent .= '		}' . "\r\n";
    $htmlContent .= '		.Message {' . "\r\n";
    $htmlContent .= '			position: fixed;' . "\r\n";
    $htmlContent .= '			width: 500px;' . "\r\n";
    $htmlContent .= '			height: 22px;' . "\r\n";
    $htmlContent .= '			flex-direction: column;' . "\r\n";
    $htmlContent .= '			flex-shrink: 0; ' . "\r\n";
    $htmlContent .= '			color: #000;' . "\r\n";
    $htmlContent .= '			font-size: 32px;' . "\r\n";
    $htmlContent .= '			font-family: Arial; ' . "\r\n";
    $htmlContent .= '			transform: translate(5%, 300%);' . "\r\n";
    $htmlContent .= '		}' . "\r\n";
    $htmlContent .= '	</style>' . "\r\n";
    $htmlContent .= '</head>' . "\r\n";
    $htmlContent .= '<body>' . "\r\n";
    $htmlContent .= '	<div class="ErrorBox">' . "\r\n";
    $htmlContent .= '		<div class="ErrorTitle">' . "\r\n";
    $htmlContent .= '			<h1>400</h>' . "\r\n";
    $htmlContent .= '		</div>' . "\r\n";
    $htmlContent .= '		<div class="Message">' . "\r\n";
    $htmlContent .= '			<p>webserver: Bad Request</p>' . "\r\n";
    $htmlContent .= '		</div>' . "\r\n";
    $htmlContent .= '	</div>' . "\r\n";
    $htmlContent .= '</body>' . "\r\n";
    $htmlContent .= '</html>' . "\r\n";

    $contentLength = strlen($htmlContent);

    $response = "HTTP/1.1 400 ERROR\r\n";
    $response .= "Content-Type: text/html; charset=UTF-8\r\n";
    $response .= "Content-Length: $contentLength\r\n";
    $response .= "\r\n";
    $response .= $htmlContent;
}

echo $response;

?>
