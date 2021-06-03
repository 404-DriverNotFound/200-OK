#include <unistd.h>
#include <iostream>
#include "../srcs/Utils/utils.hpp" // NOTE compile with "../srcs/Utils/utils.cpp"

int main()
{
	int size = 1000000;
	char buffer[size];
	std::string temp_str;
	char *temp = buffer;

	int ret = -1;
	if ((ret = read(0, temp, size)))
	{
		temp_str.append(buffer, ret);
		temp += ret;
	}
	temp[ret] = '\0';
	temp_str.pop_back();
	ft::ReplaceAll(temp_str, "1", "2");

	std::cout << "Status: 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
	std::cout << temp_str << "\r\n";
	return (1);
}