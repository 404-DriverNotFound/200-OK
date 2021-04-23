#include "../../base_headers/Path.hpp"
#include <iostream>

using namespace std;

int main()
{
	std::string temp = "index index.html is.tt sss.ss dddf";
	// std::string temp = "index index.html is.tt sss.ss dddf  ";
	// std::string temp = "  ";
	// std::string temp = "";
	
	Path path;
	path = temp;
	// path.setPath(temp);
	
	cout << path.getPath() << endl;
	
	
	return (1);
}