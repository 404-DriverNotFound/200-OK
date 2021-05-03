#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include "Connection.hpp"

class Response
{
public:
	Response(Connection* connection, int status_code, std::string body="");

	const Connection*								get_m_connection(void) const;
	const int&										get_m_status_code(void) const;
	const std::string&								get_m_status_description(void) const;
	const std::map<std::string, std::string>		get_m_headers(void) const;
	const enum TransferType&						get_m_transfer_type(void) const;
	const std::string&								get_m_content(void) const;

	void											addHeader(std::string header_key, std::string header_value);
	char*											c_str(void);
	std::map<int, std::vector<std::string> >		make_status(void);


private:
	enum TransferType								{ GENERAL, CHUNKED };

	static std::map<int, std::vector<std::string> >	status;
	Connection*										m_connection;
	int												m_status_code;
	std::string										m_status_description;
	std::map<std::string, std::string>				m_headers;
	enum TransferType								m_transfer_type;
	std::string										m_content;

};


#endif