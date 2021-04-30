#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

std::string ReplaceAll(std::string &str, const std::string& from, const std::string& to);

int main()
{
	int fd = open("./server_config_example", O_RDONLY, 0755);
	std::string a;
	char buffer[BUFSIZ];
	memset(buffer, 0, BUFSIZ);
	int ret = read(fd, buffer, BUFSIZ);
	buffer[ret] = 0;
	a = buffer;
	
	// 교환
	ReplaceAll(a, "\n", " ");
	cout << a << endl;
	ReplaceAll(a, "\t", " ");
	cout << a << endl;
	ReplaceAll(a, "  ", " ");
	ReplaceAll(a, "  ", " ");
	ReplaceAll(a, "  ", " ");
	ReplaceAll(a, "  ", " ");
	cout << a << endl;
	close(fd);
	return (1);
}

std::string ReplaceAll(std::string &str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0; //string처음부터 검사
    while((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
    }
    return str;
}
