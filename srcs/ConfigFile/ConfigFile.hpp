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
	// int parsingserverBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end, ServerBracket &ServerBracket);
	// int parsingLocationBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end);
	// int isCorrespondedServerDirective(int index, ServerBracket &ServerBracket);
public :
	// REVIEW 현재 사용하는 설정
		// NOTE - location에서 덮어씌워지는 않는 요소들
		std::string			mServerName;		// localhost
		std::string			mHost;				// def = "0.0.0.0"
		uint16_t			mPort;				// def = 8000;

		int					mTimeOut;			// def = 5s

		Path				mLocationPath;		// def = 
		// NOTE - location에서 덮어씌워지는 요소들
		bool				mAutoIndex;		// def = false;
		Path				mRoot;				// def = 
		std::vector<Path>	mIndexPages;		// def = index.html
		Path				mErrorPage;		// def = error.html
		std::vector<std::string> mMethod;		// def = GET HEAD PUT POST DELETE OPTIONS
		std::vector<std::string> mCgiProgramPath; // def = .bla
		std::vector<std::string> mCgiExtension;   // def = .bla
		Path				mAuthBasicUserFile;	// def =

		std::size_t			mClientMaxBodySize; //def 0 NOT checking size
	// REVIEW 현재 사용하지 않는 설정
		uint64_t			mHeadLengthMax;	// def = 8k // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
		uint64_t			mBodyLengthMax;	// def = 1M // REVIEW request되는 head, body의 length라면, 가변적이니깐 의미가 없지 않나 싶음
};
