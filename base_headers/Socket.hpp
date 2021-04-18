#pragma once
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.hpp"

class Socket : public sockaddr
{
	int			fd;
	socklen_t	socklen;

	public:
				Socket(uint16_t port, uint32_t ip);
				Socket(int fd);
				Socket(const Socket& x);
	Socket&		operator=(const Socket& x);
	virtual		~Socket();
	void		bind(uint16_t port, uint32_t ip);
	void		accept(int serv_sock);
	void		listen(size_t connections);

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
