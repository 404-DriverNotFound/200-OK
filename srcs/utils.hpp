#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

namespace ft
{

void	testNameSpace(void)
{
	std::cout << "namespace working" << std::endl;
}

void				bzero(void *b, size_t n)
{
	unsigned char	*dest;
	size_t			i;

	dest = (unsigned char*)b;
	i = 0;
	while (i++ < n)
		*dest++ = 0;
}

log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"))

ft::getStringFromFile(configuration_file_path) // REVIEW: ServerManager::createServer 의 config_string 을 반환한다.
ft::to_string(i) // REVIEW: ServerManager::createServer 에서 숫자를 받아 std::string 으로 숫자를 반환하는 듯?


}
#endif