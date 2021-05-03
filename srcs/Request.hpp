#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "Connection.hpp"
#include "Server.hpp"
#include "Location.hpp"


class Request
{
public:
	Request(connection, server, start_line);

	const Connection*							get_m_connection(void) const;
	const Server*								get_m_server(void) const;
	const Location*								get_m_location(void) const;
	const enum Method&							get_m_method(void) const;
	const std::string&							get_m_uri(void) const;
	const enum URIType&							get_m_uri_type(void) const;
	const std::map<std::string, std::string>&	get_m_headers(void) const;
	const enum TransferType&					get_m_transfer_type(void) const;
	const std::string&							get_m_content(void) const;
	const std::string&							get_m_origin(void) const;

	const struct timeval&						isOverTime(void) const;
	void										addContent(std::string added_content);
	void										addOrigin(std::string added_origin);
	void										addHeader(std::string header);
	void										isValidHeader(std::string header);



private:
	enum Method									{ DEFAULT, GET, HEAD, POST, PUT, DELETE, OPTIONS, TRACE };
	enum URIType								{ DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM };
	enum TransferType							{ GENERAL, CHUNKED };

	Connection*									m_connection;
	Server*										m_server;
	Location*									m_location;
	struct timeval								m_start_at;
	Method										m_method;
	std::string									m_uri;
	URIType										m_uri_type;
	std::map<std::string, std::string>			m_headers;
	TransferType								m_trasfer_type;
	std::string									m_content;
	std::string									m_origin;


};

#endif