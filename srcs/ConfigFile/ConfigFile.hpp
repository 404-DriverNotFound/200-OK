#pragma once
#include "../Path/Path.hpp"
// #include "Method.hpp"
#include <string>

class ConfigFile
{
public:
	ConfigFile();
	virtual ~ConfigFile();
	ConfigFile(const ConfigFile &);
	ConfigFile&	operator=(const ConfigFile &);
	
	// FIXME 헤더파일의 상호참조 문제를 해결해야함 #20
	// int parsingServerBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end, ServerBracket &ServerBracket);
	// int parsingLocationBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end);
	// int isCorrespondedServerDirective(int index, ServerBracket &ServerBracket);
public :
	// REVIEW 현재 사용하는 설정
		// NOTE - location에서 덮어씌워지는 않는 요소들
		std::string			mserver_name;		// localhost
		uint16_t			mport;				// def = 8000;

		bool				mauto_index;		// def = false;
		int					mtimeout;			// def = 5s

		Path				mlocation_path;		// def = 
		// NOTE - location에서 덮어씌워지는 요소들
		Path				mroot;				// def = 
		std::vector<Path>	mindex_pages;		// def = index.html
		Path				merror_page;		// def = error.html
	
	// REVIEW 현재 사용하지 않는 설정
		uint64_t			mhead_length_max;	// def = 8k // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
		uint64_t			mbody_length_max;	// def = 1M // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
		Path				mauth;				// REVIEW 뭔지 모르겠음 삭제해도 될 꺼 같음
};
