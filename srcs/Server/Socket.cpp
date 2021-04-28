#include "Socket.hpp"
#include "../Utils/utils.hpp"
#include <fstream>
#include <sstream>
/*
?? port 와 ip 로 들어온 것이 Socket 에서 무슨 역할인지 모르겠다.
Socket::Socket(uint32_t ip, uint16_t port)
{
}
*/

/* Socket 에 이런식으로 넣으면 binding, listen 작업까지 */
Socket::Socket(uint32_t ip, uint16_t port)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	SetAddr();
	Bind(ip, port);
	Listen();
}

Socket::Socket(int fd)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

Socket::~Socket()
{
	// std::cout << "~Socket()\n";
}

void					Socket::SetAddr(void)
{
	ft_memset((char*)&sockaddr, 0, sizeof(sockaddr));
}

void					Socket::Bind(uint32_t ip, uint16_t port)
{
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = ip; // localhost
	sockaddr.sin_port = htons(port); // htons is necessary to convert a number to

	int opt = 1; // 소켓을 재사용하려면 희한하게도 1로 설정해야한다.
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// network byte order
	if (bind(fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	// std::cout << "finished binding\n";
}

void					Socket::Listen(void)
{
	// Start listening. Hold at most 10 connections in the queue
	if (listen(fd, 10) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	// std::cout << "finished listening\n";
}

size_t					Socket::Accept(size_t connections)
{
	// Grab a connection from the queue
	socklen = sizeof(sockaddr);
	connections = accept(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&socklen);
	if (connections < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	// std::cout << "finished accepting\n";
	return (connections);
}

int						Socket::GetFd(void)
{
	return (fd);
}