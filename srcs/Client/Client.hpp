#pragma once
struct Client;
#include <sys/types.h>
// #include "Socket.hpp"
// #include "Buffer.hpp"
// #include "HttpReq.hpp"
// #include "HttpRes.hpp"
// #include "Cgi.hpp"
// #include "FdSet.hpp"
// #include "Server.hpp"

#define BUFFER_SIZE		16

enum e_status
{
	RECV_START_LINE,
	RECV_HEADER,
	PROC_MSG,
	SET_LOCATION,
	PROC_CGI,
	END_CGI,
	RECV_BODY,
	RECV_BODY_CHUNKED,
	MAKE_MSG,
	SEND_MSG,
	SEND_DONE
};

/*##############################################################################
Client
##############################################################################*/
struct Client
{
	/*--------------------------------------------------------------------------
	Member types
	--------------------------------------------------------------------------*/
	// typedef std::map<std::string, std::string>::iterator	header_iterator;

	/*--------------------------------------------------------------------------
	Member
	--------------------------------------------------------------------------*/
public:
	// Socket					sock;
	// Buffer					buffer;
	// std::string				line;
	// e_status				status;
	// HttpReq					req;
	// HttpRes					res;
	// Cgi						cgi;
	// std::string				path;	// 리소스 절대경로
	// Server*					server;
	// Location*				location;
	// std::vector<Server>&	vec_server;

	/*--------------------------------------------------------------------------
	Method
	--------------------------------------------------------------------------*/
				// Client(int fd, std::vector<Server>& vec_server);
				Client(const Client& x);
				~Client();
	// void		client_process(FdSet& r, FdSet& w);
	void		read_buffer();
	void		recv_start_line();
	void		recv_header();
	void		set_location();
	void		check_auth();
	void		check_method();
	void		recv_body(size_t len);
	void		recv_body_chunked();
	void		proc_cgi();
	char**		make_meta_variable();
	void		terminate_cgi();
	void		make_msg();
	// void		send_msg(FdSet& w);

	void		replace_location();
};	//Client