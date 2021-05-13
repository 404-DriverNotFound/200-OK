#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <sys/time.h>

#include "Define.hpp"
#include "Request.hpp"
class Request;
// #include "ServerManager.hpp"

class Connection
{
public:
	Connection(int client_fd, std::string client_ip, int client_port);
	Connection(void);
	
	// std::map<int, Connection>	m_connections;
	// TODO map에서 컨낵션이 끊어졌을 때, *mRequest free 해줘야함(not NULL 일 때,)
	// TODO map에서 컨낵션이 끊어지지는 않았으나(keep-alive), 한번의 요청을 처리해줫을 때( ePhase== COMPLETE), delete 후 NULL값 할당.
	virtual ~Connection(void);

	const int&				get_m_fd(void) const;
	const struct timeval&	get_m_last_request_at(void) const;
	const std::string&		get_m_client_ip(void) const;
	const int&				get_m_client_port(void) const;
	void					set_m_last_reqeust_at(void);
	
	Request*				get_m_request(void) const;
	void					set_m_request(Request* request);


	void					addRbufFromClient(char* buf, size_t count);

	//ANCHOR yunslee
	bool					isKeepConnection(void);

private:
	int						m_fd;
	struct timeval			m_last_request_at;
	std::string				m_client_ip;
	int						m_client_port;
	
	Request*				mRequest;

};

#endif