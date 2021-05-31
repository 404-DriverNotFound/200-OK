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
	, mClientMaxBodySize(0)
	, mHeadLengthMax(8000)
	, mBodyLengthMax(10000)
	, mAuth()
{
	this->mMethod.push_back("GET");
	this->mMethod.push_back("HEAD");
	this->mMethod.push_back("PUT");
	this->mMethod.push_back("POST");
	this->mMethod.push_back("DELETE");
	this->mMethod.push_back("OPTIONS");

	this->mCgiExtension.push_back(".bla");
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
	this->mAuth = ref.mAuth;

	this->mAutoIndex = ref.mAutoIndex;
	this->mTimeOut = ref.mTimeOut;
	this->mMethod = ref.mMethod;
	this->mCgiExtension = ref.mCgiExtension;
	this->mClientMaxBodySize = ref.mClientMaxBodySize;
	return (*this);
}
