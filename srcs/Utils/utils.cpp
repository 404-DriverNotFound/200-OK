#include "utils.hpp"

void	*ft_memset(void *b, int c, size_t len)
{
	while (len--)
		*((unsigned char*)b + len) = (unsigned char)c;
	return (b);
}

void ft_split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find(delim, before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// NOTE usr/yunslee/index.html 의 경우 마지막 index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}
