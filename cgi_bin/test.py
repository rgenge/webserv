import os

if "Body-Path" in os.environ and os.environ["Body-Path"] != "":
	if "Content-Type" in os.environ and os.environ["Content-Type"] == "text/plain":
		file_path = os.environ["Body-Path"]
		with open(file_path, "r") as file:
			text = file.read()
else:
    text = ""

htmlContent = """<!DOCTYPE html>
<html>
<head>
	<title>200 OK</title>
</head>
<body>
	<h1>Successful request</h1>
	<p>{}</p>
</body>
</html>\r\n\r\n""".format(text)

contentLength = len(htmlContent)

response = """HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n

{}""".format(contentLength, htmlContent)

print(response)