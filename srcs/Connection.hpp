#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <sys/time.h>

#include "Define.hpp"
#include "Request.hpp"
#include "Response.hpp"
class Request;
class Response;

class Connection
{
public:
	enum Status { REQUEST_READY, REQUEST_ING, CGI_READY, CGI_ING, SEND_READY, SEND_ING};

	Connection(const int& clientFd, const std::string& clientIp, const int& clientPort);
	Connection(void);

	virtual ~Connection(void);

	const int&					GetSocket(void) const;
	const std::string&			GetClientIp(void) const;
	const int&					GetClientPort(void) const;
	void						SetLastReqeustAt(const struct timeval& time);
	
	Request*					GetRequest(void) const;
	void						SetRequest(Request* request);
	Response*					GetResponse(void) const;
	void						SetResponse(Response* response);

	const Status&				GetStatus(void) const;
	void						SetStatus(const Status& status);

	const std::string&			GetTargetUri(void) const;
	void						SetTargetUri(std::string& targetUri);

	const std::string&			GetServerName(void) const;
	void						SetServerName(std::string& serverName);

	const std::string&			GetCgiProgramPath(void) const;
	void						SetCgiProgramPath(std::string& cgiProgramPath);

	void						AddRbufFromClient(const char* buf, const size_t& count);

	bool						IsKeepConnection(void);

	private:
		const struct timeval&	getLastRequestAt(void) const;

		std::string				mClientIp;
		int						mClientPort;
		int						mFd;
		Request*				mRequest;
		Response*				mResponse;
		Status					eStatus;
		struct timeval			mLastRequestAt;
		std::string				mTargetUri;
		std::string				mServerName;
		std::string				mCgiProgramPath;
};

#endif