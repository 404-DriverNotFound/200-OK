#include "Connection.hpp"

Connection::Connection(int clientFd, std::string clientIp, int clientPort)
	: mClientIp(clientIp)
	, mClientPort(clientPort)
	, mFd(clientFd)
	, mRequest(0)
	, mHttpMessage(0)
	, mStatus(REQUEST_READY)
{
	gettimeofday(&(this->mLastRequestAt), NULL);
}

Connection::Connection(void)
	: mClientIp("")
	, mClientPort(0)
	, mFd(0)
	, mRequest(0)
	, mHttpMessage(0)
	, mStatus(REQUEST_READY)
{
	gettimeofday(&(this->mLastRequestAt), NULL);
}

Connection::~Connection(void)
{
	if (this->mRequest != NULL)
	{
		delete (this->mRequest);
		this->mRequest = NULL;
	}
	if (this->mHttpMessage != NULL)
	{
		delete (this->mHttpMessage);
		this->mHttpMessage = NULL;
	}
}


const int&					Connection::getSocket(void) const{return (this->mFd);}

const struct timeval&		Connection::getLastRequestAt(void) const{	return (this->mLastRequestAt);}

const std::string&			Connection::getClientIp(void) const{	return (this->mClientIp);}

const int&					Connection::getClientPort(void) const{	return (this->mClientPort);}

void						Connection::setLastReqeustAt(const struct timeval& time)
{
	mLastRequestAt = time;
}

bool						Connection::isKeepConnection(void) //TODO 연결된 순간 m_last_request를 갱신해야함.
{
	struct timeval	now;
	gettimeofday(&now, NULL);

	struct timeval	before;
	before = this->mLastRequestAt;
	if (before.tv_sec + KEEP_ALIVE_LIMIT < now.tv_sec)
		return (false);
	else
		return (true);
}

Request*					Connection::getRequest(void) const
{
	return (mRequest);
}

void						Connection::setRequest(Request* request)
{
	mRequest = request;
}

void						Connection::addRbufFromClient(char* buf, size_t count)
{
	mRequest->addHttpMessage(std::string(buf, count));
}


Response*					Connection::getResponse(void) const
{
	return (this->mHttpMessage);
}

void						Connection::setResponse(Response* response)
{
	this->mHttpMessage = response;
}

const Connection::eStatus&	Connection::GetStatus(void) const
{
	return (mStatus);
}

void						Connection::SetStatus(const eStatus& status)
{
	mStatus = status;
}