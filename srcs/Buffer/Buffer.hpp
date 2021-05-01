#pragma once
#include <deque>
#include <string>
#include <unistd.h>
#include "Utils.hpp"

/*##############################################################################
Buffer
##############################################################################*/
struct				Buffer
{
	/*--------------------------------------------------------------------------
	Member
	--------------------------------------------------------------------------*/
	int				fd;
	ssize_t			buffer_size;
	char*			buffer;
	char*			cursor;
	char*			end;
	int				read_request;		// 소켓의 버퍼를 읽어야 하는 상태
	int				is_token_complete;
	ssize_t			write_request;		// cgi에 쓸 것이 남은 상태
	ssize_t			len;

	/*--------------------------------------------------------------------------
	Method
	--------------------------------------------------------------------------*/
					Buffer();
					Buffer(int fd, size_t buffer_size = 0x100000);
					Buffer(const Buffer& x);
	Buffer&			operator=(const Buffer& x);
	virtual			~Buffer();
	void			get_token(std::string& token, int sep);
	void			get_token_seq(std::string& token, char* seq);
	char			get_token_set(std::string& token, char* set);
	int				read_buffer();
	ssize_t			size() const;
	void			write(size_t s, int fd);
};