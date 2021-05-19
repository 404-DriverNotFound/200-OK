#!/bin/bash

export "AUTH_TYPE="
export "CONTENT_LENGTH=4"
export "CONTENT_TYPE="
export "GATEWAY_INTERFACE=CGI/1.1"
export "PATH_INFO=aaaaaa"
export "PATH_TRANSLATED=./file.bla"
export "QUERY_STRING=./baba"
export "REMOTE_ADDR="
export "REMOTE_IDENT="
export "REMOTE_USER="
export "REQUEST_METHOD=PUT"
export "REQUEST_URI="
export "SCRIPT_NAME="
export "SERVER_NAME="
export "SERVER_PORT="
export "SERVER_PROTOCOL=HTTP/1.1"
export "SERVER_SOFTWARE="

echo "abcde" | ./cgi_tester
