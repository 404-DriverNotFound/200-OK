#pragma once
#include "Path.hpp"
#include "Method.hpp"
#include <string>

class Config
{
public:
	Config();
	virtual ~Config();
	Config(const Config &);
	Config &operator=(const Config &);
	
	// FIXME 헤더파일의 상호참조 문제를 해결해야함 #20
	// int parsingServerBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end, ServerBracket &ServerBracket);
	// int parsingLocationBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end);
	// int isCorrespondedServerDirective(int index, ServerBracket &ServerBracket);

public :
	// REVIEW 아래 멤버들은 필수적인 파트
	std::string			mserver_name;		// localhost
	Path				mroot;				// def = 
	Path				mlocation_path;		// def = 
	uint16_t			mport;				// def = 80;
	std::vector<Path>	mindex_pages;		// def = index.html
	Path				merror_page;			// def = error.html
	
	// REVIEW server config 멤버로써 구성해야하는 부분인가?
	uint64_t			mhead_length_max;	// def = 8k // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
	uint64_t			mbody_length_max;	// def = 1M // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
	bool				mautoindex;			// def = off
	uint32_t			mtimeout;			// def = 5s
	Path				mauth;				// REVIEW 뭔지 모르겠음 삭제해도 될 꺼 같음
};