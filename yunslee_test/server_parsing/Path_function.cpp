#include <iostream>
#include <string>
#include <vector>
using namespace std;


void setPath(std::vector<std::string> &vec, const std::string& str)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find("/", before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		cout << "   str: " << str << endl;
		cout << "substr: " << substr << endl;
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

const string getPath(std::vector<std::string> &vec)
{
	std::string temp;
	for (size_t i = 0; i < vec.size(); i++)
	{
		temp.append(vec[i]);
		if (i != vec.size() - 1)
			temp += "/";
	}
	return (temp);
}


int main()
{
	std::string a;
	a = "//a/////"; // 지금 담기긴 함. --> 에러처리 할 필요성이 있음
	// a = "/";
	// a = "////";
	// a = "a/b/c/";
	a = "a/b/c";
	std::vector<std::string> b;
	setPath(b, a);
	std::string temp = getPath(b);
	cout << temp << endl;
	return (1);
}
