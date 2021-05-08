#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

class Connection
{
public:
	Connection(int client_fd,std::string client_ip,int client_port);

	const int&				get_m_fd(void) const;
	const struct timeval&	get_m_last_request_at(void) const;
	const std::string&		get_m_client_ip(void) const;
	const int&				get_m_client_port(void) const;
	const int&				set_m_last_reqeust_at(void) const;

private:
	int				m_fd;
	struct timeval	m_last_request_at;
	std::string		m_client_ip;
	int				m_client_port;

};

#endif