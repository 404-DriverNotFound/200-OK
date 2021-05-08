#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int buf_size = 200000000; // INT_MAX 까지만 write함수의 세번째인자에 들어감
	char *buff_rcv = (char *)malloc(sizeof(char) * buf_size);
	char file_name[200]; memset(file_name, 0, 200);
	sprintf(file_name, "%dbyte", buf_size);
	std::cout << file_name << std::endl;
	int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (fd < 0){perror("open_error: ");return (0);}
	std::cout << write(fd, buff_rcv, buf_size) << std::endl;
	close(fd);
	std::cout << "-= " << errno << " : "<< std::strerror(errno) << " =-" << std::endl;
	return (1);
}
