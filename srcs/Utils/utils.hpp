#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>

namespace ft
{
	void	testNameSpace(void);
	void	*memset(void *b, int c, size_t len);
	void	split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim);
}

#endif