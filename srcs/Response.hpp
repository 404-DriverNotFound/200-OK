#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Define.hpp"

#include <string>
#include <map>
// #include "Connection.hpp"
#include "Utils/utils.hpp"

class Connection;

enum TransferType								{ GENERAL, CHUNKED };

class Response
{
private:
	Response(); // NOTE 인자 없이 생성되어 Connection class에 연결되는 일이 없도록 하자.
public:
	virtual ~Response();
	Response(Connection* connection, int statusCode, std::string body = "");

	const Connection*								getConnection(void) const;
	const int&										getStatusCode(void) const;
	const std::string&								getStatusDescription(void) const;
	const std::string&								getFirstLine(void) const;
	std::map<std::string, std::string>&				getHeaders(void);
	const enum TransferType&						getTransferType(void) const;
	const std::string&								getBody(void) const;

	const std::string&								getResponse(void) const;

	void		setResponse(std::string);
	void		setConnection(Connection *connect);
	void		setStatusCode(int statusCode);
	void		setStatusDescription(std::string &status_description);
	void		setHeaders(std::string header_key, std::string header_value);
	void		setTransferType(enum TransferType);
	void		setBody(const std::string& body);

	void		ShowMessage(void);

	void		clearHeaders();
	void		makeFirstLine();
	void		copyHeaders(std::map<std::string, std::string> &ref);

	// NOTE runSend 에서 보낼 것은 makeResponse()이 함수 내용을 보내면 끝!!!! 나지 않는다. 소켓이라 소켓송신버퍼이상 byte는 한번에 보낼 수 없다.
	const std::string								makeResponse(void);

	// ANCHOR yunslee static 함수(error page 관련 함수)
	// REVIEW const 처리를 해주는 것이 좋아보임.
	static std::string								makeStatusPage(int statusCode, std::string method);
	static std::map<int, std::string >				mStatusMap;
	static void				initStatusMap(void)
	{
		mStatusMap[0] = "Undefined Error";

		mStatusMap[100] = "Continue";
		mStatusMap[101] = "Switching Protocols";
		mStatusMap[103] = "Early Hints";
		mStatusMap[200] = "OK";
		mStatusMap[201] = "Created";
		mStatusMap[202] = "Accepted";
		mStatusMap[203] = "Non-Authoritative Information";
		mStatusMap[204] = "No Content";
		mStatusMap[205] = "Reset Content";
		mStatusMap[206] = "Partial Content";
		mStatusMap[301] = "Moved Permanently";
		mStatusMap[300] = "Multiple Choices";
		mStatusMap[303] = "See Other";
		mStatusMap[302] = "Found";
		mStatusMap[307] = "Temporary Redirect";
		mStatusMap[304] = "Not Modified";
		mStatusMap[400] = "Bad Request";
		mStatusMap[308] = "Permanent Redirect";
		mStatusMap[402] = "Payment Required";
		mStatusMap[401] = "Unauthorized";
		mStatusMap[404] = "Not Found";
		mStatusMap[403] = "Forbidden";
		mStatusMap[406] = "Not Acceptable";
		mStatusMap[405] = "Method Not Allowed";
		mStatusMap[408] = "Request Timeout";
		mStatusMap[407] = "Proxy Authentication Required";
		mStatusMap[410] = "Gone";
		mStatusMap[409] = "Conflict";
		mStatusMap[412] = "Precondition Failed";
		mStatusMap[411] = "Length Required";
		mStatusMap[414] = "URI Too Long";
		mStatusMap[413] = "Payload Too Large";
		mStatusMap[416] = "Range Not Satisfiable";
		mStatusMap[415] = "Unsupported Media Type";
		mStatusMap[418] = "I'm a teapot";
		mStatusMap[417] = "Expectation Failed";
		mStatusMap[425] = "Too Early";
		mStatusMap[422] = "Unprocessable Entity";
		mStatusMap[428] = "Precondition Required";
		mStatusMap[426] = "Upgrade Required";
		mStatusMap[431] = "Request Header Fields Too Large";
		mStatusMap[429] = "Too Many Requests";
		mStatusMap[500] = "Internal Server Error";
		mStatusMap[451] = "Unavailable For Legal Reasons";
		mStatusMap[502] = "Bad Gateway";
		mStatusMap[501] = "Not Implemented";
		mStatusMap[504] = "Gateway Timeout";
		mStatusMap[503] = "Service Unavailable";
		mStatusMap[506] = "Variant Also Negotiates";
		mStatusMap[505] = "HTTP Version Not Supported";
		mStatusMap[508] = "Loop Detected";
		mStatusMap[507] = "Insufficient Storage";
		mStatusMap[511] = "Network Authentication Required";
		mStatusMap[510] = "Not Extended";
	}

private:
	Connection*										mConnection;
	
	std::string										mFirstLine;
	int												mStatusCode;
	std::string										mStatusDescription;
	std::map<std::string, std::string>				mHeaders;
	enum TransferType								eTransferType;
	std::string										mBody;

	std::string										mResponse;
};

#endif