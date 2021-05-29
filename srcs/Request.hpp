#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "Define.hpp"
#include "Connection.hpp"
class Connection;
#include "Server.hpp"
class Server;

class Request
{
public:
	enum eURIType								{ DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM };
	enum eTransferType							{ GENERAL, CHUNKED };
	enum ePhase									{ READY, ON_HEADER, ON_BODY, COMPLETE };

	Request(void);

	std::map<std::string, std::string>&			GetHeaders(void);

	const eTransferType&						GetTransferType(void) const;
	void										setTransferType(const eTransferType& trasferType);

	const std::string&							GetBody(void) const;
	void										SetBody(const std::string& body);
	
	const std::string&							getMessage(void) const;
	void										SetOrigin(const std::string& origin);

	const ePhase&								GetPhase(void) const;
	void										SetPhase(const ePhase& phase);

	const std::string&							GetMethod(void) const;
	void										SetMethod(const std::string& method);


	const std::size_t&							GetSeek(void) const;
	void										SetSeek(const std::size_t& seek);

	const std::string&							GetVersion(void) const;
	void										SetVersion(const std::string& version);

	void										AddBody(const std::string& body);
	void										AddMessage(const std::string& added_origin);
	void										AddHeader(const std::string& header);
	bool										IsValidHeader(const std::string& header);

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

	const struct timeval&						GetStartTime(void) const;
	void										SetStartTime(const struct timeval& startTime);

	void										ShowMessage(void);
private:
	struct timeval								mStartTime;

	ePhase										mPhase;
	std::size_t									mSeek;
	std::string									mMessage;

	std::string									mMethod;
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