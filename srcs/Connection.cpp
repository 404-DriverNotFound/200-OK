#include "Connection.hpp"

Connection::Connection(int client_fd, std::string client_ip, int client_port)
	: mIP(client_ip)
	, mPort(client_port)
	, mSocket(client_fd)
	, mRequest(0)
	, mResponse(0)
	, mStatus(REQUEST_READY)
{
	gettimeofday(&(this->mLastRequestAt), NULL);
}

Connection::Connection(void)
	: mIP("")
	, mPort(0)
	, mSocket(0)
	, mRequest(0)
	, mResponse(0)
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
	if (this->mResponse != NULL)
	{
		delete (this->mResponse);
		this->mResponse = NULL;
	}
}


const int&					Connection::GetSocket(void) const{return (this->mSocket);}

const struct timeval&		Connection::GetLastRequestAt(void) const{	return (this->mLastRequestAt);}

const std::string&			Connection::GetIP(void) const{	return (this->mIP);}

const int&					Connection::GetPort(void) const{	return (this->mPort);}

void						Connection::SetLastRequestAt(const struct timeval& time)
{
	mLastRequestAt = time;
}

bool						Connection::IsKeepConnection(void) //TODO 연결된 순간 m_last_request를 갱신해야함.
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

Request*					Connection::GetRequest(void) const
{
	return (mRequest);
}

void						Connection::SetRequest(Request* request)
{
	mRequest = request;
}

void						Connection::AddRbufFromClient(char* buf, size_t count)
{
	mRequest->AddMessage(std::string(buf, count));
}


Response*					Connection::GetResponse(void) const
{
	return (this->mResponse);
}

void						Connection::SetResponse(Response* response)
{
	this->mResponse = response;
}

const Connection::eStatus&	Connection::GetStatus(void) const
{
	return (mStatus);
}

void						Connection::SetStatus(const eStatus& status)
{
	mStatus = status;
}