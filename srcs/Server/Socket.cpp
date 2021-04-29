#include "Socket.hpp"
#include "../Utils/utils.hpp"
#include <fstream>
#include <sstream>

/* Socket 에 이런식으로 넣으면 binding, listen 작업까지 */
Socket::Socket(uint32_t ip, uint16_t port)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw (socket_failed_exception());
	SetAddr();
	Bind(ip, port);
	Listen();
}

Socket::Socket(int fd)
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw (socket_failed_exception());
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
	if (bind(fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		throw (bind_failed_exception());
	// std::cout << "finished binding\n";
}

void					Socket::Listen(void)
{
	// Start listening. Hold at most 10 connections in the queue
	if (listen(fd, 10) < 0)
		throw (listen_failed_exception());
	// std::cout << "finished listening\n";
}

size_t					Socket::Accept(size_t connections)
{
	// Grab a connection from the queue
	socklen = sizeof(sockaddr);
	connections = accept(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&socklen);
	if (connections < 0)
		throw (accept_failed_exception());
	// std::cout << "finished accepting\n";
	return (connections);
}

int						Socket::GetFd(void)
{
	return (fd);
}

/*************************************************************
exceptions
**************************************************************/

const char* Socket::socket_failed_exception::what() const throw()
{
	return ("Failed to create socket.");
};

const char* Socket::bind_failed_exception::what() const throw()
{
	return ("Failed to bind to port.");
};

const char* Socket::listen_failed_exception::what() const throw()
{
	return ("Failed to listen on socket.");
};

const char* Socket::accept_failed_exception::what() const throw()
{
	return ("Failed to grab connection.");
};
