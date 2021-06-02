#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

enum byte
{
	eMB_1,
	eMB_100,
	eGB_1,
	eKB_500
};

int KB_500 =  500000; // INT_MAX 까지만 write함수의 세번째인자에 들어감
int MB_1 =   1000000; // INT_MAX 까지만 write함수의 세번째인자에 들어감
int MB_100 = 100000000; // INT_MAX 까지만 write함수의 세번째인자에 들어감
int GB_1 = 	1000000000; // INT_MAX 까지만 write함수의 세번째인자에 들어감

int main()
{
	printf("0번: 1MB\n1번: 100MB\n2번: 1GB\n");
	write(1, "입력해주세요: ", strlen("입력해주세요: "));
	int flag;
	scanf("%d", &flag);
	int buf_size;
	std::string filename;
	switch (flag)
	{
		case eMB_1:
			buf_size = MB_1;
			filename = "1MB.txt";
			break;
		case eMB_100:
			buf_size = MB_100;
			filename = "100MB.txt";
			break;
		case eGB_1:
			buf_size = GB_1;
			filename = "1GB.txt";
			break;
		case eKB_500:
			buf_size = KB_500;
			filename = "500KB.txt";
			break;
		default:
			break;
	}
	char *buffer = (char *)malloc(sizeof(char) * buf_size);
	memset(buffer, 'a', buf_size);
	std::cout << filename.c_str() << std::endl;
	int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (fd < 0){perror("open_error: ");return (0);}
	std::cout << write(fd, buffer, buf_size) << std::endl;
	close(fd);
	std::cout << "-= " << errno << " : "<< std::strerror(errno) << " =-" << std::endl;
	return (1);
}