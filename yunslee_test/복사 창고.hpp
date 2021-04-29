#include <iostream>
#include <string>

using namespace std;

const int eDirective_num = 12;
enum eDirective
{
	ROOT, SERVER_NAME, PORT, ERROR_PAGE, INDEX,
	HEAD_LENGTH, BODY_LENGTH, AUTOINDEX, TIMEOUT, AUTH,
	LOCATION, METHOD
};

std::string directives[12] = {"root ", "server_name ", "port ", "erro_page ", "index ",
									"head_length ", "body_length ", "autoindex ", "timeout ", "auth "
										, "location ", "method "};