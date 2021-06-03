#pragma once
#include "../Path/Path.hpp"
#include <string>

class ConfigFile
{
public:
	ConfigFile();
	virtual ~ConfigFile();
	ConfigFile(const ConfigFile &);
	ConfigFile&	operator=(const ConfigFile &);
	
public :
		// REVIEW 현재 사용하는 설정
		// NOTE - location에서 덮어씌워지는 않는 요소들
		std::string					mServerName;			// localhost
		std::string					mHost;					// def = "0.0.0.0"
		uint16_t					mPort;					// def = 8000;
		int							mTimeOut;				// def = 0s
		Path						mLocationPath;			// def = 
		// NOTE - location에서 덮어씌워지는 요소들
		bool						mAutoIndex;				// def = false;
		Path						mRoot;					// def = 
		std::vector<Path>			mIndexPages;			// def = index.html
		Path						mErrorPage;				// def = 
		std::vector<std::string>	mMethod;				// def = GET HEAD PUT POST DELETE OPTIONS
		std::vector<std::string>	mCgiProgramPath;		// def = /bin/etc/php ft_tester/bla_tester
		std::vector<std::string>	mCgiExtension;			// def = .bla .php
		Path						mAuthBasicUserFile;		// def =
		std::size_t					mClientMaxBodySize;		//def 0 NOT checking size
		// REVIEW 현재 사용하지 않는 설정
		uint64_t			mHeadLengthMax;	// def = 8k
		uint64_t			mBodyLengthMax;	// def = 1M
};
