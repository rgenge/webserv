# web-server

## About

Baisc HTTP server following in HTTP/1.1 standard in C++98.

## Config file

It works like nginx config files, you have one in the server you can use, you can either change it to try it out. `conf/`.

- `server`: same as nginx server block.
- `location`: same as nginx location block.
- `listen`: `ip:port`: ip:port to listen on, you specify only the ip, or only the port, or none; defualt values are `0.0.0.0:80`.
- `server_name`: list of names that will be used to determine the server if more than one server is listening on the same ip:port, useful for virtual hosting.

A brief explanetion of server/location blocks directives:

- `root`: root folder of the server.
- `index`: index file that should return when client makes a GET request to the directory
- `autoindex`: `on/off`: autoindex page to choose if you want to access the server as folders
- `error_page`: `404 example.html`: specify a custom html file for an error code.
- `methods`: `GET POST DELETE`: specify allowed methods on that server/location block.
- `body_size_limit`: max size for a client request body.
- `upload`: path where uploaded POST  files will be stored at.
- `cgi`: p.e.: `cgi .php /usr/bin/php`, specify a CGI binary for a file extension.

## How to use

### Compile
```sh
make
```
### Execute
```sh
./webserv [CONF FILE]
```

##

