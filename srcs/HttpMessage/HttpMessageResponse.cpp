#include <iostream>	//	FIXME	나중에 지우기 테스트용
#include <fstream>	//	FIXME	나중에 지우기 테스트용
#include <iomanip>	//	FIXME	나중에 지우기 테스트용
#include <cstdlib>	//	FIXME	나중에 지우기 테스트용
#include "HttpMessageResponse.hpp"

HttpMessageResponse::HttpMessageResponse(const HttpMessageRequest& request)
	: mRequest(request)
{
	// if (mReasonPhrase.empty())
	// {
	// 	mReasonPhrase[100] = "Continue";
	// 	mReasonPhrase[101] = "Switching Protocols";
	// 	mReasonPhrase[103] = "Early Hints";
	// 	mReasonPhrase[200] = "OK";
	// 	mReasonPhrase[201] = "Created";
	// 	mReasonPhrase[202] = "Accepted";
	// 	mReasonPhrase[203] = "Non-Authoritative Information";
	// 	mReasonPhrase[204] = "No Content";
	// 	mReasonPhrase[205] = "Reset Content";
	// 	mReasonPhrase[206] = "Partial Content";
	// 	mReasonPhrase[300] = "Multiple Choices";
	// 	mReasonPhrase[301] = "Moved Permanently";
	// 	mReasonPhrase[302] = "Found";
	// 	mReasonPhrase[303] = "See Other";
	// 	mReasonPhrase[304] = "Not Modified";
	// 	mReasonPhrase[307] = "Temporary Redirect";
	// 	mReasonPhrase[308] = "Permanent Redirect";
	// 	mReasonPhrase[400] = "Bad Request";
	// 	mReasonPhrase[401] = "Unauthorized";
	// 	mReasonPhrase[402] = "Payment Required";
	// 	mReasonPhrase[403] = "Forbidden";
	// 	mReasonPhrase[404] = "Not Found";
	// 	mReasonPhrase[405] = "Method Not Allowed";
	// 	mReasonPhrase[406] = "Not Acceptable";
	// 	mReasonPhrase[407] = "Proxy Authentication Required";
	// 	mReasonPhrase[408] = "Request Time-out";
	// 	mReasonPhrase[409] = "Conflict";
	// 	mReasonPhrase[410] = "Gone";
	// 	mReasonPhrase[411] = "Length Required";
	// 	mReasonPhrase[412] = "Precondition Failed";
	// 	mReasonPhrase[413] = "Request Entity Too Large";
	// 	mReasonPhrase[414] = "Request-URI Too Large";
	// 	mReasonPhrase[415] = "Unsupported Media Type";
	// 	mReasonPhrase[416] = "Requested range not satisfiable";
	// 	mReasonPhrase[417] = "Expectation Failed";
	// 	mReasonPhrase[418] = "I'm a teapot";
	// 	mReasonPhrase[422] = "Unprocessable Entity";
	// 	mReasonPhrase[425] = "Too Early";
	// 	mReasonPhrase[426] = "Upgrade Required";
	// 	mReasonPhrase[428] = "Precondition Required";
	// 	mReasonPhrase[429] = "Too Many Requests";
	// 	mReasonPhrase[431] = "Request Header Fields Too Large";
	// 	mReasonPhrase[451] = "Unavailable For Legal Reasons";
	// 	mReasonPhrase[500] = "Internal Server Error";
	// 	mReasonPhrase[501] = "Not Implemented";
	// 	mReasonPhrase[502] = "Bad Gateway";
	// 	mReasonPhrase[503] = "Service Unavailable";
	// 	mReasonPhrase[504] = "Gateway Time-out";
	// 	mReasonPhrase[505] = "HTTP Version not supported";
	// 	mReasonPhrase[506] = "Variant Also Negotiates";
	// 	mReasonPhrase[507] = "Insufficient Storage";
	// 	mReasonPhrase[508] = "Loop Detected";
	// 	mReasonPhrase[510] = "Not Extended";
	// 	mReasonPhrase[511] = "Network Authentication Required";
	// }
}

HttpMessageResponse::~HttpMessageResponse(void)
{
}

void				HttpMessageResponse::setStatusLine(void)
{
	// int	statusCode = 200;
	mMessage += "HTTP/1.1";
	mMessage += " ";
	// mMessage += itoa(statusCode);	//	REVIEW : itoa 허용함수 인지
	mMessage += "200";
	mMessage += " ";
	// mMessage += mReasonPhrase[200];
	mMessage += "OK";
	mMessage += "\r\n";
}

void				HttpMessageResponse::setHeaderField(void)
{
}

void				HttpMessageResponse::setMessageBody(void)
{
	mMessage += "<h1>Hello World</h1>";
	mMessage += "<h1>This is 200 OK!</h1>";
}

void				HttpMessageResponse::SetMessage(void)
{
	setStatusLine();
	setHeaderField();
	mMessage += "\r\n";
	setMessageBody();
}

const std::string&	HttpMessageResponse::GetMessage(void) const
{
	return (mMessage);
}