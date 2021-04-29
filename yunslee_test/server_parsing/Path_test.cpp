#include "../../base_headers/Path.hpp"
#include <iostream>

using namespace std;

int main()
{
	std::string temp1 = "index/index.html/is.tt/sss.ss/dddf";
	std::string temp2 = "/index/index.html/is.tt/sss.ss/dddf  ";
	std::string temp3 = "//";
	std::string temp4 = "/";
	std::string temp5 = "";
	std::string temp6 = "asdf";
	
	Path path;
	Path path2;
	path = temp1;
	path2 = temp6;
	
	path = path + path2;
	
	cout << path.getSize() << endl;
	for (size_t i = 0; i < path.getSize(); i++)
	{
		cout << path.getPath() << endl;
	}
	
	
	int size = path.getSize();
	for (size_t i = 0; i < size; i++)
	{
		path--;
		cout << path.getPath() << endl;
	}
	
	
	return (1);
}