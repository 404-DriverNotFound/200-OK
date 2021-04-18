#pragma once
#include <string>
#include <map>
#include <deque>
#include "Method.hpp"
#include "Url.hpp"
#include "Path.hpp"
#include "utils.hpp"

// class StartLine
// {
// 	public:
// 	std::string		protocol;
// 					StartLine();
// 					StartLine(const std::string& str);
// 					StartLine(const StartLine& x);
// 	StartLine&		operator=(const StartLine& x);
// 	virtual			~StartLine();
// 	std::string		get_protocol() const;
// };

// class StartLineReq : public StartLine
// {
// 	public:
// 	e_method		method;
// 	Path			path;
// 					StartLineReq();
// 					StartLineReq(const StartLineReq& x);
// 					StartLineReq(const std::string& str);
// 	virtual			~StartLineReq();
// 	std::string		get_protocol() const;
// };

// class StartLineRes : public StartLine
// {
// 	public:
// 	uint8_t			code;
// 					StartLineRes();
// 					StartLineRes(const StartLineRes& x);
// 					StartLineRes(const std::string& str);
// 	virtual			~StartLineRes();
// 	std::string		get_protocol() const;
// };

// enum						e_message_type
// {
// 	REQ,
// 	RES,
// };

// class StartLine
// {
// 	public:
// 	e_message_type				message_type;
// 	std::vector<std::string>	tokens;
// 								StartLine();
// 								StartLine(e_message_type t, std::string& str);
// 								StartLine(const StartLine& x);
// 	StartLine&					operator=(const StartLine& x);
// 	virtual						~StartLine();
// 	std::string					get_protocol() const;
// };

class Http
{
	std::multimap<std::string, std::string>	headers;
	std::deque<uint8_t>						body;
	std::string								protocol;
	int										version_major;
	int										version_minor;
};

class HttpReq : public Http
{
	e_method								method;
	std::string								path;
};

class HttpRes : public Http
{
	int										status_code;
	std::map<int, std::string>*				map_code;
};
