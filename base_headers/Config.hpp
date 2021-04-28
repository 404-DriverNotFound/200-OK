#pragma once
#include "Path.hpp"
#include "Method.hpp"
#include <string>


class Config
{
	public :
		std::string			server_name;		// localhost
		Path				root;				// def = /
		Path				location_path;		// def = /
		uint16_t			port;				// def = 80;
		std::vector<Path>	index_pages;		// def = index.html
		Path				error_page;			// def = error.html
		// REVIEW 위에 부분은 필수적인 파트
		
		// REVIEW server config 멤버로써 구성해야하는 부분인가?
		uint64_t			head_length_max;	// def = 8k // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
		uint64_t			body_length_max;	// def = 1M // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
		bool				autoindex;			// def = off
		uint32_t			timeout;			// def = 5s
		Path				auth;				// REVIEW 뭔지 모르겠음 삭제해도 될 꺼 같음

	public:
		Config();
		virtual ~Config();
		Config(const Config &);
		Config &operator=(const Config &);
};