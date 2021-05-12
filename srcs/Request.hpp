#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Define.hpp"

#include <string>
#include <map>
#include "Connection.hpp"
class Connection;

#include "Server.hpp"
class Server;

#include "Location.hpp"

class Request
{
public:
	enum eMethod								{ DEFAULT, GET, HEAD, POST, PUT, DELETE, OPTIONS, TRACE };
	// enum eURIType								{ DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM };
	// enum eTransferType							{ GENERAL, CHUNKED };
	enum ePhase									{ READY, ON_HEADER, ON_BODY, COMPLETE };

	Request(void);
	// Request(Connection* connection, Server* server, std::string start_line);

	// const Connection*							get_m_connection(void) const;
	// const Server*								get_m_server(void) const;
	// const Location*								get_m_location(void) const;
	const eMethod&								get_m_method(void) const;
	const std::string&							get_m_uri(void) const;
	// const eURIType&								get_m_uri_type(void) const;
	// const std::map<std::string, std::string>&	get_m_headers(void) const;
	// const eTransferType&						get_m_transfer_type(void) const;
	// const std::string&							get_m_content(void) const;
	
	const std::string&							get_m_origin(void) const;
	void										SetOrigin(const std::string& origin);

	const ePhase&								GetPhase(void) const;
	void										SetPhase(const ePhase& phase);

	void										set_m_method(const eMethod& method);
	void										set_m_uri(const std::string& uri);

	// const struct timeval&						isOverTime(void) const;
	// void										addContent(std::string added_content);
	void										addOrigin(const std::string& added_origin);
	void										addHeader(const std::string& header);
	bool										isValidHeader(const std::string& header);


private:
	// Connection*									m_connection;
	// Server*										m_server;
	// Location*									m_location;
	// struct timeval								m_start_at;
	eMethod										m_method;
	std::string									m_uri;
	// eURIType									m_uri_type;
	std::map<std::string, std::string>			m_headers;
	// eTransferType								m_trasfer_type;
	// std::string									m_content;
	std::string									m_origin;
	ePhase										mPhase;

};

#endif