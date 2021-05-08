#include "Connection.hpp"

Connection::Connection(int client_fd, std::string client_ip, int client_port) :
	m_client_ip(client_ip), m_client_port(client_port), m_fd(client_fd)
{
	gettimeofday(&(this->m_last_request_at), NULL);
}

Connection::Connection() :
	m_client_ip(""), m_client_port(0), m_fd(0)
{
	gettimeofday(&(this->m_last_request_at), NULL);
}

const int&				Connection::get_m_fd(void) const{return (this->m_fd);}

const struct timeval&	Connection::get_m_last_request_at(void) const{	return (this->m_last_request_at);}

const std::string&		Connection::get_m_client_ip(void) const{	return (this->m_client_ip);}

const int&				Connection::get_m_client_port(void) const{	return (this->m_client_port);}

void					Connection::set_m_last_reqeust_at(void)
{
	gettimeofday(&(this->m_last_request_at), NULL);
	return ;
}
