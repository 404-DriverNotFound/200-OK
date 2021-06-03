#include "Response.hpp"

Response::Response()
{
}

Response::~Response()
{
}

Response::Response(Connection* connection, int statusCode, std::string body)
	: mConnection(connection), mStatusCode(statusCode), mBody(body), mSeek(0), mHttpMessageLength(0)
{
	this->setConnection(connection);
	this->setStatusCode(statusCode);
	this->makeFirstLine();	
	this->setTransferType(GENERAL);
	this->setBody(body);
	this->setHeaders("Content-Length", ft::itos(body.length()));
}

const Connection*								Response::getConnection(void) const{return (this->mConnection);}

const int&										Response::getStatusCode(void) const{return (this->mStatusCode);}

const std::string&								Response::getStatusDescription(void) const{return (this->mStatusDescription);}

std::map<std::string, std::string>&				Response::getHeaders(void) {return (this->mHeaders);}

const enum TransferType&						Response::getTransferType(void) const{	return (this->eTransferType);}

const std::string&								Response::GetBody(void) const{	return (this->mBody);}

const std::string&								Response::getFirstLine(void) const { return (this->mFirstLine);}

const std::string&								Response::GetHttpMessage(void) const {return (this->mHttpMessage);};


const std::string								Response::makeHttpMessage(void)
{
	std::string all;
	
	all += this->mFirstLine;
	all += "\r\n";
	std::map<std::string, std::string>::iterator it = mHeaders.begin();
	while (it != mHeaders.end())
	{
		all += it->first;
		all += ": ";
		all += it->second;
		all += "\r\n";
		it++;
	}
	all += "\r\n";
	all += this->mBody;
	return (all);
}

std::string										Response::makeStatusPage(int statusCode, std::string method)
{
	std::string errorpage;
	errorpage += "<html><head><title>statusCode STATUS_DESCRIPTION</title></head><body bgcolor=\"white\"><center>\n";
	errorpage += "<h1>statusCode STATUS_DESCRIPTION</h1></center><hr>\n";
	errorpage += "<h2>Method: METHOD</h2></center><hr>\n";
	errorpage += "<center>YKK_Webserver</center></body></html>";
	
	ft::ReplaceAll(errorpage, "statusCode", ft::itos(statusCode));
	ft::ReplaceAll(errorpage, "STATUS_DESCRIPTION", Response::mStatusMap[statusCode]);
	ft::ReplaceAll(errorpage, "METHOD", method);
	return (errorpage);
}


void		Response::setHttpMessage(const std::string& message)
{
	this->mHttpMessage = message;
}

void		Response::setConnection(Connection *connect)
{
	this->mConnection = connect;
}

void		Response::setStatusCode(int statusCode)
{
	this->mStatusCode = statusCode;
}

void		Response::setStatusDescription(std::string &status_description)
{
	this->mStatusDescription = status_description;
}

void		Response::makeFirstLine()
{
	this->mFirstLine += "HTTP/1.1 ";
	this->mFirstLine += ft::itos(mStatusCode);
	this->mFirstLine += " ";
	this->mFirstLine += mStatusMap[mStatusCode];
}

void		Response::setHeaders(std::string header_key, std::string header_value)
{
	this->mHeaders[header_key] = header_value;
}

void		Response::setTransferType(enum TransferType type)
{
	this->eTransferType = type;
}

void		Response::setBody(const std::string& body)
{
	this->mBody = body;
}

void		Response::clearHeaders()
{
	this->mHeaders.clear();
}

void		Response::copyHeaders(std::map<std::string, std::string> &ref)
{
	this->clearHeaders();
	std::map<std::string, std::string>::iterator it = ref.begin();
	while (it != ref.end())
	{
		this->mHeaders[it->first] = it->second;
		it++;
	}
}

void		Response::ShowMessage(void)
{
	// status line
	std::cout << "HTTP/1.1 " << mStatusCode << " " << mStatusMap[mStatusCode] << std::endl;

	// response header
	for (std::map<std::string, std::string>::iterator it = getHeaders().begin(); it != getHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << std::endl;

	// body
	if (GetBody().empty() == false)
	{
		std::cout << GetBody() << std::endl;
	}
}

const std::size_t&					Response::GetSeek(void) const
{
	return (mSeek);
}

void								Response::SetSeek(const std::size_t& seek)
{
	mSeek = seek;
}

const std::size_t&					Response::GetResponseLength(void) const
{
	return (mHttpMessageLength);
}

void								Response::SetResponseLength(const std::size_t& length)
{
	mHttpMessageLength = length;
}
