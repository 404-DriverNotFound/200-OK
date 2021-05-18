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


	void		set_m_connection(Connection *connect);
	void		set_m_status_code(int status_code);
	void		set_m_status_description(std::string &status_description);
	void		set_m_headers(std::string header_key, std::string header_value);
	void		set_m_transfer_type(enum TransferType);
	void		set_m_body(std::string &body);

	void		clear_m_headers();
	void		make_m_firstline();
	void		copy_m_headers(std::map<std::string, std::string> &ref);

	// NOTE runSend 에서 보낼 것은 getResponse()이 함수 내용을 보내면 끝!!!!
	const std::string								getResponse(void);

	// ANCHOR yunslee static 함수(error page 관련 함수)
	// REVIEW const 처리를 해주는 것이 좋아보임.
	static std::string								makeStatusPage(int status_code, std::string method);
	static std::map<int, std::string >				m_status_map;
	static std::map<std::string, std::string>		m_MIME_map;
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
	static void init_MIME_map(void)
	{
		m_MIME_map[".aac"] = "audio/aac";
		m_MIME_map[".abw"] = "application/x-abiword";
		m_MIME_map[".arc"] = "application/octet-stream";
		m_MIME_map[".avi"] = "video/x-msvideo";
		m_MIME_map[".azw"] = "application/vnd.amazon.ebook";
		m_MIME_map[".bin"] = "application/octet-stream";
		m_MIME_map[".bz"] = "application/x-bzip";
		m_MIME_map[".bz2"] = "application/x-bzip2";
		m_MIME_map[".csh"] = "application/x-csh";
		m_MIME_map[".css"] = "text/css";
		m_MIME_map[".csv"] = "text/csv";
		m_MIME_map[".doc"] = "application/msword";
		m_MIME_map[".epub"] = "application/epub+zip";
		m_MIME_map[".gif"] = "image/gif";
		m_MIME_map[".htm"] = "text/html";
		m_MIME_map[".html"] = "text/html";
		m_MIME_map[".ico"] = "image/x-icon";
		m_MIME_map[".ics"] = "text/calendar";
		m_MIME_map[".jar"] = "Temporary Redirect";
		m_MIME_map[".jpeg"] = "image/jpeg";
		m_MIME_map[".jpg"] = "image/jpeg";
		m_MIME_map[".js"] = "application/js";
		m_MIME_map[".json"] = "application/json";
		m_MIME_map[".mid"] = "audio/midi";
		m_MIME_map[".midi"] = "audio/midi";
		m_MIME_map[".mpeg"] = "video/mpeg";
		m_MIME_map[".mpkg"] = "application/vnd.apple.installer+xml";
		m_MIME_map[".odp"] = "application/vnd.oasis.opendocument.presentation";
		m_MIME_map[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
		m_MIME_map[".odt"] = "application/vnd.oasis.opendocument.text";
		m_MIME_map[".oga"] = "audio/ogg";
		m_MIME_map[".ogv"] = "video/ogg";
		m_MIME_map[".ogx"] = "application/ogg";
		m_MIME_map[".pdf"] = "application/pdf";
		m_MIME_map[".ppt"] = "application/vnd.ms-powerpoint";
		m_MIME_map[".rar"] = "application/x-rar-compressed";
		m_MIME_map[".rtf"] = "application/rtf";
		m_MIME_map[".sh"] = "application/x-sh";
		m_MIME_map[".svg"] = "image/svg+xml";
		m_MIME_map[".swf"] = "application/x-shockwave-flash";
		m_MIME_map[".tar"] = "application/x-tar";
		m_MIME_map[".tif"] = "image/tiff";
		m_MIME_map[".tiff"] = "image/tiff";
		m_MIME_map[".ttf"] = "application/x-font-ttf";
		m_MIME_map[".vsd"] = " application/vnd.visio";
		m_MIME_map[".wav"] = "audio/x-wav";
		m_MIME_map[".weba"] = "audio/webm";
		m_MIME_map[".webm"] = "video/webm";
		m_MIME_map[".webp"] = "image/webp";
		m_MIME_map[".woff"] = "application/x-font-woff";
		m_MIME_map[".xhtml"] = "application/xhtml+xml";
		m_MIME_map[".xls"] = "application/vnd.ms-excel";
		m_MIME_map[".xml"] = "application/xml";
		m_MIME_map[".xul"] = "application/vnd.mozilla.xul+xml";
		m_MIME_map[".zip"] = "application/zip";
		m_MIME_map[".3gp"] = "video/3gpp audio/3gpp";
		m_MIME_map[".3g2"] = "video/3gpp2 audio/3gpp2";
		m_MIME_map[".7z"] = "application/x-7z-compressed";
	}

private:
	Connection*										m_connection;
	
	std::string										m_firstline;
	int												m_status_code;
	std::string										m_status_description;
	std::map<std::string, std::string>				m_headers;
	enum TransferType								m_transfer_type;
	std::string										m_body;
	
};

#endif