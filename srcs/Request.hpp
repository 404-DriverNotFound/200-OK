/*
******************** Request *********************
Request 정보들을 저장한다. 단일 쓰레드에서는 Request가 한 번에 하나씩 완전히 처리되기 때문에 다른 객체에 의해 관리되지 않는다.

*/

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include "Connection.hpp"
#include "Server.hpp"
#include "Location.hpp"


class Request
{
public:
	Request(Connection& connection, Server& server, std::string& start_line); // REVIEW: start_line 이 string 일 지 모르겠음.

	/* getters */
	const Connection*							get_m_connection(void) const;
	const Server*								get_m_server(void) const;
	const Location*								get_m_location(void) const;
	const enum Method&							get_m_method(void) const;
	const std::string&							get_m_uri(void) const;
	const enum URIType&							get_m_uri_type(void) const;
	const std::map<std::string, std::string>&	get_m_headers(void) const;
	const enum TransferType&					get_m_transfer_type(void) const;
	const std::string&							get_m_content(void) const;
	const std::string&							get_m_origin(void) const;

	const struct timeval&						isOverTime(void) const; // NOTE: request 가 시작한지 오래되었는지 확인
	void										addContent(std::string added_content); // NOTE: m_content 끝에 인자 string 을 붙여넣음
	void										addOrigin(std::string added_origin); // NOTE: m_origin 끝에 인자 string 을 붙여넣음
	void										addHeader(std::string header); // NOTE: header 라인 1줄을 파싱하여 m_headers에 저장
	void										isValidHeader(std::string header); // NOTE: header 라인 1줄의 형식 유효성과 중복여부 검사



private:
	enum Method									{ DEFAULT, GET, HEAD, POST, PUT, DELETE, OPTIONS, TRACE }; // NOTE: method 열거
	enum URIType								{ DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM }; // NOTE: uri type 열거
	enum TransferType							{ GENERAL, CHUNKED }; // NOTE: Transfer type 열거

	Connection*									m_connection; // NOTE: request가 송신되는 커넥션 객체 참조
	Server*										m_server; // NOTE: request가 처리되는 서버 객체 참조
	Location*									m_location; // NOTE: request uri가 속해있는 location 참조
	struct timeval								m_start_at; // NOTE: request 처리가 시작된 시각 저장
	Method										m_method; // NOTE: request method
	std::string									m_uri; // NOTE: request uri
	URIType										m_uri_type; // NOTE: request uri type
	std::map<std::string, std::string>			m_headers; // NOTE: request message의 header 저장
	TransferType								m_trasfer_type; // NOTE: request transfer type
	std::string									m_content; // NOTE: request body
	std::string									m_origin; // NOTE: request message 원본 저장, string, TRACE 메소드에 대한 응답 목적


};

#endif