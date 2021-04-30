#include <iostream>
#include <string>
#include <vector>
#include <string.h>

using namespace std;

void ft_split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find(delim, before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		// cout << "   str: " << str << endl;
		// cout << "substr: " << substr << endl;
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// usr/yunslee/index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}


int main()
{
	
	const char *str = {"index index.html index.htmaal index.htm"};
	
	std::string temp; temp = str;
	std::vector<std::string> temp_vector;
	ft_split_vector(temp_vector, temp, " ");
	
	for (size_t i = 0; i < temp_vector.size(); i++)
	{
		cout << temp_vector[i] << endl;
	}
	return (1);
}

