#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <cstring>

using namespace std;
int buf_size = 1024;

int main(void)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == server_socket)
	{
		write(1, "server socket error\n", 20);
		exit(1);
	}
	int value = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
	{
		write(1, "server socket opt error\n", 24);
        return (0);
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000); // port 번호는 endian 변환
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (-1 == bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) || errno != 0)
	{
		write(1, "bind error\n", 11);
		exit(1);
	}
	if (-1 == listen(server_socket, 5) || errno != 0)
	{
		write(1, "listen error\n", 13);
		exit(1);
	}

	struct sockaddr_in client_addr;
	memset(&server_addr, 0, sizeof(client_addr));
	unsigned int client_addr_size = sizeof(client_addr);
	int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);

	if (-1 == client_socket)
	{
		write(1, "client socket error\n", 20);
		exit(1);
	}

	// ANCHOR 여기서부터 시작
	int		read_ret = 0;
	int		sum = 0;
	timeval temp;
	buf_size = 300005000; //300MB
	char *buff_rcv = (char *)malloc(buf_size * sizeof(char));
	// memset(buff_rcv, 0, buf_size); // 10ms 정도 걸림 -> TCP 소켓한테 시간을 주는거라고 생각함
	
	// NOTE 1. NON_BLOCKING
	// int flags = fcntl(client_socket, F_GETFL, 0);
	// flags |= O_NONBLOCK;	
	// fcntl(client_socket, F_SETFL, flags);

	// NOTE 2. sleep() 60 Operation timed out
	// usleep(1);

	// ANCHOR 1. read() BLOCKING
	std::cout << "read: " << (read_ret = read(client_socket, buff_rcv, buf_size)) << std::endl;
	std::cout << "-= errno: " << errno << " " << std::strerror(errno) << " =-" << std::endl;

	// ANCHOR 2. read() NONBLOCK
	char *temp2 = buff_rcv;
	while (0 != (read_ret = read(client_socket, temp2, buf_size)))
	{
		// NOTE 3. usleep() 버퍼에 더 많이 담을려면 stream이 더 많이 들어도록 기다려야한다.
		// usleep(1000);
		std::cout << "-= errno: " << errno << " " << std::strerror(errno) << " =-" << std::endl;
		errno = 0;
		if (read_ret == -1)
		{
			std::cout << "read_ret: -1" << std::endl;
			continue;
		}
		else
		{
			std::cout << "read_ret: " << read_ret << std::endl;
			temp2 += read_ret; sum+= read_ret;
		}
	}
	temp2[read_ret] = '\0';

	std::cout << "sum: " << sum <<std::endl;

	close(server_socket);
	close(client_socket);
	return (0);
}