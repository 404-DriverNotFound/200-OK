#include "Response.hpp"

Response::Response()
{
	// InitStatusMap(); 초기화는 메인 문에서 한번 해주고 있음. 안해줘도 됨
}

Response::~Response()
{
	
}

Response::Response(Connection* connection, int status_code, std::string body)
	: mConnection(connection), mStatusCode(status_code), mBody(body)
{
	// InitStatusMap(); 초기화는 메인 문에서 한번 해주고 있음. 안해줘도 됨
	this->setConnection(connection);
	this->setStatusCode(status_code); // NOTE status_code 인자값이 잘못된 인자 값이 들어오진 않는다 Server클래스의 함수를 통해 생성되기 때문에!
	this->makeFirstLine();	
	this->setTransferType(GENERAL); // REVIEW chunked 일때, 값을 넣어줘야함.
	this->SetBody(body);
	this->SetHeaders("Content-Length", ft::itos(body.length()));
}

const Connection*								Response::GetConnection(void) const{return (this->mConnection);}

const int&										Response::GetStatusCode(void) const{return (this->mStatusCode);}

const std::string&								Response::GetStatusDescription(void) const{return (this->mStatusDescription);}

std::map<std::string, std::string>&				Response::GetHeaders(void) {return (this->mHeaders);}

const enum TransferType&						Response::GetTransferType(void) const{	return (this->mTransferType);}

const std::string&								Response::GetBody(void) const{	return (this->mBody);}

const std::string&								Response::GetFirstLine(void) const { return (this->mFirstLine);}

const std::string&								Response::GetResponse(void) const {return (this->mResponse);};


const std::string								Response::MakeResponse(void)
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
	// std::cout << all << std::endl;
	return (all);
}

std::string										Response::MakeStatusPage(int status_code, std::string method)
{
	std::string errorpage;
	errorpage += "<html><head><title>STATUS_CODE STATUS_DESCRIPTION</title></head><body bgcolor=\"white\"><center>\n";
	errorpage += "<h1>STATUS_CODE STATUS_DESCRIPTION</h1></center><hr>\n";
	errorpage += "<h2>Method: METHOD</h2></center><hr>\n";
	errorpage += "<center>YKK_Webserver</center></body></html>";
	
	ft::ReplaceAll(errorpage, "STATUS_CODE", ft::itos(status_code));
	ft::ReplaceAll(errorpage, "STATUS_DESCRIPTION", Response::mStatusMap[status_code]);
	ft::ReplaceAll(errorpage, "METHOD", method);
	return (errorpage);
}


void		Response::SetResponse(std::string response)
{
	this->mResponse = response;
}

void		Response::setConnection(Connection *connect)
{
	this->mConnection = connect;
}

void		Response::setStatusCode(int status_code)
{
	this->mStatusCode = status_code;
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
	this->mFirstLine += this->mStatusDescription;
}

void		Response::SetHeaders(std::string header_key, std::string header_value)
{
	this->mHeaders[header_key] = header_value;
}

void		Response::setTransferType(enum TransferType type)
{
	this->mTransferType = type;
}

void		Response::SetBody(const std::string& body)
{
	this->mBody = body;
}

void		Response::clearHeaders()
{
	this->mHeaders.clear();
}

void		Response::CopyHeaders(std::map<std::string, std::string> &ref)
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
	std::cout << "HTTP/9.9 " << mStatusCode << " " << mStatusMap[mStatusCode] << std::endl;

	// response header
	for (std::map<std::string, std::string>::iterator it = GetHeaders().begin(); it != GetHeaders().end(); ++it)
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