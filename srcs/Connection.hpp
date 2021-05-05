/*
**************** Connection *****************
Server 소켓으로 들어오는 연결에 관한 정보들을 저장한다. Server 객체의 멤버 변수로 사용된다

*/

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

class Connection
{
public:
	Connection(int client_fd, uint32_t client_ip, int client_port);

	const int&				get_m_fd(void) const;
	const struct timeval&	get_m_last_request_at(void) const;
	const std::string&		get_m_client_ip(void) const;
	const int&				get_m_client_port(void) const;
	const int&				set_m_last_reqeust_at(void) const;
	// get_m_request(void) const;

private:
	int				m_fd; // NOTE: 클라이언트 fd
	struct timeval	m_last_request_at; // NOTE: 최근 request 시각
	std::string		m_client_ip; // NOTE: 클라이언트 ip
	int				m_client_port; // NOTE: 클라이언트 port

};

#endif