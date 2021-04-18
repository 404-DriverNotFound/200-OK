#include <iostream>
#include <string>
using namespace std;

int main()
{
	std::string a;
	int count = BUFSIZ;
	char buffer[BUFSIZ];
	memset(buffer, '1', BUFSIZ);
	buffer[BUFSIZ - 1] = 0;
	a.append(buffer);
	for (size_t i = 0; i < count; i++)
	{
		if (i % 3 == 0)
		{
			a.insert(i, "\n");
			a.replace(a.find("\n"), 1, "22");
		}
	}
	// a.replace(a.find("\r", 1, " "))
	
	// int count = 100000;
	// for (size_t i = 0; i < count; i++)
	// {
	// 	a.append("c", 1);
	// }
	
	cout << a << endl;
	cout << a.size() << endl;

	// try
	// {
	// 	while (1)
	// 	{
	// 		a.append("c", 1);
	// 	}
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	cout << a.length() << endl;
	// }
	
	// cout << a << endl;
	// cout << sizeof(std::string) << endl;
	return (1);
}
