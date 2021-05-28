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
	Response(Connection* connection, int status_code, std::string body = "");

	const Connection*								get_m_connection(void) const;
	const int&										get_m_status_code(void) const;
	const std::string&								get_m_status_description(void) const;
	const std::string&								get_m_firstline(void) const;
	std::map<std::string, std::string>&				get_m_headers(void);
	const enum TransferType&						get_m_transfer_type(void) const;
	const std::string&								get_m_body(void) const;

	const std::string&								get_m_response(void) const;

	void		set_m_response(std::string);
	void		set_m_connection(Connection *connect);
	void		set_m_status_code(int status_code);
	void		set_m_status_description(std::string &status_description);
	void		set_m_headers(std::string header_key, std::string header_value);
	void		set_m_transfer_type(enum TransferType);
	void		set_m_body(const std::string& body);

	void		ShowMessage(void);

	void		clear_m_headers();
	void		make_m_firstline();
	void		copy_m_headers(std::map<std::string, std::string> &ref);

	// NOTE runSend 에서 보낼 것은 makeResponse()이 함수 내용을 보내면 끝!!!! 나지 않는다. 소켓이라 소켓송신버퍼이상 byte는 한번에 보낼 수 없다.
	const std::string								makeResponse(void);

	// ANCHOR yunslee static 함수(error page 관련 함수)
	// REVIEW const 처리를 해주는 것이 좋아보임.
	static std::string								makeStatusPage(int status_code, std::string method);
	static std::map<int, std::string >				m_status_map;
	static void				init_status_map(void)
	{
		m_status_map[0] = "Undefined Error";

		m_status_map[100] = "Continue";
		m_status_map[101] = "Switching Protocols";
		m_status_map[103] = "Early Hints";
		m_status_map[200] = "OK";
		m_status_map[201] = "Created";
		m_status_map[202] = "Accepted";
		m_status_map[203] = "Non-Authoritative Information";
		m_status_map[204] = "No Content";
		m_status_map[205] = "Reset Content";
		m_status_map[206] = "Partial Content";
		m_status_map[301] = "Moved Permanently";
		m_status_map[300] = "Multiple Choices";
		m_status_map[303] = "See Other";
		m_status_map[302] = "Found";
		m_status_map[307] = "Temporary Redirect";
		m_status_map[304] = "Not Modified";
		m_status_map[400] = "Bad Request";
		m_status_map[308] = "Permanent Redirect";
		m_status_map[402] = "Payment Required";
		m_status_map[401] = "Unauthorized";
		m_status_map[404] = "Not Found";
		m_status_map[403] = "Forbidden";
		m_status_map[406] = "Not Acceptable";
		m_status_map[405] = "Method Not Allowed";
		m_status_map[408] = "Request Timeout";
		m_status_map[407] = "Proxy Authentication Required";
		m_status_map[410] = "Gone";
		m_status_map[409] = "Conflict";
		m_status_map[412] = "Precondition Failed";
		m_status_map[411] = "Length Required";
		m_status_map[414] = "URI Too Long";
		m_status_map[413] = "Payload Too Large";
		m_status_map[416] = "Range Not Satisfiable";
		m_status_map[415] = "Unsupported Media Type";
		m_status_map[418] = "I'm a teapot";
		m_status_map[417] = "Expectation Failed";
		m_status_map[425] = "Too Early";
		m_status_map[422] = "Unprocessable Entity";
		m_status_map[428] = "Precondition Required";
		m_status_map[426] = "Upgrade Required";
		m_status_map[431] = "Request Header Fields Too Large";
		m_status_map[429] = "Too Many Requests";
		m_status_map[500] = "Internal Server Error";
		m_status_map[451] = "Unavailable For Legal Reasons";
		m_status_map[502] = "Bad Gateway";
		m_status_map[501] = "Not Implemented";
		m_status_map[504] = "Gateway Timeout";
		m_status_map[503] = "Service Unavailable";
		m_status_map[506] = "Variant Also Negotiates";
		m_status_map[505] = "HTTP Version Not Supported";
		m_status_map[508] = "Loop Detected";
		m_status_map[507] = "Insufficient Storage";
		m_status_map[511] = "Network Authentication Required";
		m_status_map[510] = "Not Extended";
	}

private:
	Connection*										m_connection;
	
	std::string										m_firstline;
	int												m_status_code;
	std::string										m_status_description;
	std::map<std::string, std::string>				m_headers;
	enum TransferType								m_transfer_type;
	std::string										m_body;

	std::string										m_response;
};

#endif