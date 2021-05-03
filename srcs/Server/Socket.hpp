#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> // <-- htons 추가하였음.
#include "../Utils/utils.hpp"

/* 김포프 컨벤션으로 바꿈 */
class Socket : public sockaddr // REVIEW sockaddr 구조체는 쓰지않고 sockaddr_in으로 충분하기 때문에 상속관계를 지워도 될 꺼 같다.
{
	int			fd;
	socklen_t	socklen;
	sockaddr_in	sockaddr; // <-- 추가함.

	public:
				Socket();
				Socket(uint32_t ip, uint16_t port);
				Socket(int fd);
				// Socket(const Socket& x);
	// Socket&		operator=(const Socket& x);
	virtual		~Socket();
	void		MemsetAddr(void);
	int 		SetSocket(uint32_t ip, uint16_t port);
	void		Bind(uint32_t ip, uint16_t port);
	void		Listen(void);
	// void		Listen(size_t connections); // connection 은 뭘까?
	size_t		Accept(size_t connections);
	// void		Accept(int servSock); // servSock 은 뭘까?
	int			GetFd(void); // socket fd 를 close 를 main 에서 하기 위한

	// exceptions
	class socket_failed_exception : public std::exception{
		virtual const char*	what() const throw();	};
	class bind_failed_exception : public std::exception{
		virtual const char*	what() const throw();	};
	class accept_failed_exception : public std::exception{
		virtual const char*	what() const throw();	};
	class listen_failed_exception : public std::exception{
		virtual const char*	what() const throw();	};
};

// class Socket : public sockaddr
// {
// 	int			fd;
// 	socklen_t	socklen;

// 	public:
// 				Socket(uint16_t port, uint32_t ip);
// 				Socket(int fd);
// 				Socket(const Socket& x);
// 	Socket&		operator=(const Socket& x);
// 	virtual		~Socket();
// 	void		bind(uint16_t port, uint32_t ip);
// 	void		accept(int serv_sock);
// 	void		listen(size_t connections);

// 	// exceptions
// 	class socket_failed_exception : public std::exception{
// 		virtual const char*	what() const throw();	};
// 	class bind_failed_exception : public std::exception{
// 		virtual const char*	what() const throw();	};
// 	class accept_failed_exception : public std::exception{
// 		virtual const char*	what() const throw();	};
// 	class listen_failed_exception : public std::exception{
// 		virtual const char*	what() const throw();	};
// };
