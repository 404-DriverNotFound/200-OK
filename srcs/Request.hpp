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
	enum eURIType								{ DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM };
	enum eTransferType							{ GENERAL, CHUNKED };
	enum ePhase									{ READY, ON_HEADER, ON_BODY, COMPLETE };

	Request(void);
	// Request(Connection* connection, Server* server, std::string start_line);

	// const Connection*							get_m_connection(void) const;
	// const Server*								get_m_server(void) const;
	// const Location*								get_m_location(void) const;

	std::map<std::string, std::string>&			GetHeaders(void);

	const eTransferType&						GetTransferType(void) const;
	void										SetTransferType(const eTransferType& trasferType);

	const std::string&							getBody(void) const;
	void										SetBody(const std::string& body);
	
	const std::string&							get_m_origin(void) const;
	void										SetOrigin(const std::string& origin);

	const ePhase&								GetPhase(void) const;
	void										SetPhase(const ePhase& phase);

	const eMethod&								GetMethod(void) const;
	void										SetMethod(const eMethod& method);


	const std::size_t&							GetSeek(void) const;
	void										SetSeek(const std::size_t& seek);

	const std::string&							GetVersion(void) const;
	void										SetVersion(const std::string& version);

	// const struct timeval&						isOverTime(void) const;
	void										addBody(const std::string& body);
	void										addOrigin(const std::string& added_origin);
	void										addHeader(const std::string& header);
	bool										isValidHeader(const std::string& header);

	// URI 관련된 친구들 //
	void										ParseURI(std::string& uri);
	const std::string&							GetURI(void) const;
	void										SetURI(const std::string& uri);
	const std::string&							GetDirectory(void) const;
	void										SetDirectory(const std::string& directory);
	const std::string&							GetFileName(void) const;
	void										SetFileName(const std::string& fileName);
	const std::string&							GetParameter(void) const;
	void										SetParameter(const std::string& parameter);
	const std::string&							GetQuery(void) const;
	void										SetQuery(const std::string& query);
	const std::string&							GetFragment(void) const;
	void										SetFragment(const std::string& fragment);
	const eURIType&								GetURItype(void) const;
	void										SetURItype(const eURIType& uriType);
	// URI 관련된 친구들 //

private:
	// Connection*									m_connection;
	// Server*										m_server;
	// Location*									m_location;

	// struct timeval								m_start_at;

	ePhase										mPhase;
	std::size_t									mSeek;
	std::string									m_origin;	//FIXME http message 전문을 담고 있음 이부분은 나중에 수정해야함

	eMethod										mMethod;
	/* uri 친구들 */
	std::string									mURI;
	std::string									mDirectory;
	std::string									mFileName;
	std::string									mParameter;
	std::string									mQuery;
	eURIType									mURItype;
	/* uri 친구들 */
	std::string									mVersion;

	std::map<std::string, std::string>			mHeaders;
	std::string									mBody;

	eTransferType								mTransferType;
};

#endif