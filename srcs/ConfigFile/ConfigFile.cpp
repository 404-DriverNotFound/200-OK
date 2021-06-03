#include "./ConfigFile.hpp"

ConfigFile::ConfigFile()
	: mServerName("localhost")
	, mHost("0.0.0.0")
	, mPort(8000)
	, mTimeOut(0)
	, mLocationPath()
	, mAutoIndex(false)
	, mRoot()
	, mIndexPages()
	, mErrorPage()
	, mAuthBasicUserFile()
	, mClientMaxBodySize(0)
	, mHeadLengthMax(8000)
	, mBodyLengthMax(10000)
{
	this->mMethod.push_back("GET");
	this->mMethod.push_back("HEAD");
	this->mMethod.push_back("PUT");
	this->mMethod.push_back("POST");
	this->mMethod.push_back("DELETE");
	this->mMethod.push_back("OPTIONS");

	this->mCgiProgramPath.push_back("/usr/bin/php");
	this->mCgiProgramPath.push_back("ft_tester/bla_tester");

	this->mCgiExtension.push_back(".bla");
	this->mCgiExtension.push_back(".php");
}

ConfigFile::~ConfigFile(){}

ConfigFile::ConfigFile(const ConfigFile &ref)
{
	this->operator=(ref);
}

ConfigFile &ConfigFile::operator=(const ConfigFile &ref)
{
	if (this == &ref)
		return (*this);
	this->mServerName = ref.mServerName;
	this->mRoot = ref.mRoot;
	this->mHost = ref.mHost;
	this->mLocationPath = ref.mLocationPath;
	this->mPort = ref.mPort;
	this->mIndexPages = ref.mIndexPages;
	this->mErrorPage = ref.mErrorPage;
	this->mHeadLengthMax = ref.mHeadLengthMax;
	this->mBodyLengthMax = ref.mBodyLengthMax;
	this->mAuthBasicUserFile = ref.mAuthBasicUserFile;

	this->mAutoIndex = ref.mAutoIndex;
	this->mTimeOut = ref.mTimeOut;
	this->mMethod = ref.mMethod;
	this->mCgiProgramPath = ref.mCgiProgramPath;
	this->mCgiExtension = ref.mCgiExtension;
	this->mClientMaxBodySize = ref.mClientMaxBodySize;
	return (*this);
}
