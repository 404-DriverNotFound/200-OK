#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <limits.h> // FIXME atoi 쓸꺼면 필요함.

namespace ft
{
	void			testNameSpace(void);
	void*			memset(void *b, int c, size_t len);
	void			split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim);
	std::string		ReplaceAll(std::string &str, const std::string& from, const std::string& to);
	// int				atoi(const char *str);
	char*			itoa(int n);
	int				atoi(const char *str);

	bool isFilePath(const std::string &path);
	bool isDirPath(const std::string &path);
	u_int32_t ft_htonl(u_int32_t ip_addr);
	u_int16_t ft_htons(u_int16_t port);
	in_addr_t ft_inet_addr(const char * ip_address);
}

#endif