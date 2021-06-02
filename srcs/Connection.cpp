#include "Connection.hpp"

Connection::Connection(const int& clientFd, const std::string& clientIp, const int& clientPort)
	: mClientIp(clientIp)
	, mClientPort(clientPort)
	, mFd(clientFd)
	, mRequest(0)
	, mResponse(0)
	, eStatus(REQUEST_READY)
	, mTargetUri("")
{
	gettimeofday(&(this->mLastRequestAt), NULL);
}

Connection::Connection(void)
	: mClientIp("")
	, mClientPort(0)
	, mFd(0)
	, mRequest(0)
	, mResponse(0)
	, eStatus(REQUEST_READY)
	, mTargetUri("")
{
	gettimeofday(&(this->mLastRequestAt), NULL);
}

Connection::~Connection(void)
{
	if (mRequest != NULL)
	{
		delete (mRequest);
		mRequest = NULL;
	}
	if (mResponse != NULL)
	{
		delete (mResponse);
		mResponse = NULL;
	}
}

const int&					Connection::GetSocket(void) const	{return (this->mFd);}

const struct timeval&		Connection::getLastRequestAt(void) const	{return (this->mLastRequestAt);}

const std::string&			Connection::GetClientIp(void) const	{return (this->mClientIp);}

const int&					Connection::GetClientPort(void) const	{return (this->mClientPort);}

void						Connection::SetLastReqeustAt(const struct timeval& time)
{
	mLastRequestAt = time;
}

bool						Connection::IsKeepConnection(void)
{
	struct timeval	now;
	gettimeofday(&now, NULL);

	struct timeval	before;
	before = this->mLastRequestAt;
	if (before.tv_sec + KEEP_ALIVE_LIMIT < now.tv_sec)
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

Request*					Connection::GetRequest(void) const
{
	return (mRequest);
}

void						Connection::SetRequest(Request* request)
{
	mRequest = request;
}

void						Connection::AddRbufFromClient(const char* buf, const size_t& count)
{
	mRequest->AddHttpMessage(std::string(buf, count));
}


Response*					Connection::GetResponse(void) const
{
	return (mResponse);
}

void						Connection::SetResponse(Response* response)
{
	mResponse = response;
}

const Connection::Status&	Connection::GetStatus(void) const
{
	return (eStatus);
}

void						Connection::SetStatus(const Status& status)
{
	eStatus = status;
}

const std::string&			Connection::GetTargetUri(void) const
{
	return (mTargetUri);
}

void						Connection::SetTargetUri(std::string& targetUri)
{
	this->mTargetUri = targetUri;
}