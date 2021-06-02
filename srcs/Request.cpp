#include "Request.hpp"

Request::Request(void)
	: mPhase(READY)
	, mSeek(0)
	, mURItype(DIRECTORY)
	, mTransferType(GENERAL)
{
	gettimeofday(&mStartTime, NULL);
}

void								Request::AddHttpMessage(const std::string& added)
{
	mHttpMessage.append(added);
}

const std::string&					Request::GetHttpMessage(void) const
{
	return (mHttpMessage);
}

void								Request::SetHttpMessage(const std::string& message)
{
	mHttpMessage = message;
}

const std::string&					Request::GetMethod(void) const
{
	return (mMethod);
}

void								Request::SetMethod(const std::string& method)
{
	mMethod = method;
}

const Request::ePhase&				Request::GetPhase(void) const
{
	return (mPhase);
}

void								Request::SetPhase(const ePhase& phase)
{
	mPhase = phase;
}

void								Request::AddHeader(const std::string& header)
{
	std::size_t	found = header.find(": ");
	if (found == std::string::npos)
	{
		throw 400;
	}
	std::string	key = header.substr(0, found);
	std::locale loc;
	for(std::string::size_type i = 0; i < key.length(); i++)
	{
		key[i] = std::tolower(key[i], loc);
	}

	std::string	value = header.substr(found + 2);

	if (key.compare("transfer-encoding") == 0)
	{
		if (value.find("chunked") != std::string::npos)
		{
			SetTransferType(CHUNKED);
		}
	}

	if (key.compare("host") == 0)
	{
		std::size_t	found = value.find(":");
		if (found != std::string::npos)
		{
			value = value.substr(0, found);
		}
	}
	mHeaders.insert(std::pair<std::string, std::string>(key, value));
}

const std::string&					Request::GetBody(void) const
{
	return (mBody);
}

void								Request::SetBody(const std::string& body)
{
	mBody = body;
}

void								Request::AddBody(const std::string& body)
{
	mBody.append(body);
}

const std::size_t&					Request::GetSeek(void) const
{
	return (mSeek);
}

void								Request::SetSeek(const std::size_t& seek)
{
	mSeek = seek;
}

std::map<std::string, std::string>&	Request::GetHeaders(void)
{
	return (mHeaders);
}

const std::string&					Request::GetVersion(void) const
{
	return (mVersion);
}
void								Request::SetVersion(const std::string& version)
{
	mVersion = version;
}

const Request::eTransferType&		Request::GetTransferType(void) const
{
	return (mTransferType);
}

void								Request::SetTransferType(const eTransferType& trasferType)
{
	mTransferType = trasferType;
}

void								Request::ParseURI(std::string& uri)
{
	// http://localhost:8000/2019/08/index.html;page=1?isEdit=true&id=123#fragment
	std::size_t	found;
	mURI = uri;

	// NOTE 414
	if (mURI.length() > 200)
	{
		throw 414;
	}

	// query parsing
	found = uri.find_last_of("?");
	if (found != std::string::npos)
	{
		mQuery = uri.substr(found + 1);
		uri.resize(found);
	}
	// parameter parsing
	found = uri.find_last_of(";");
	if (found != std::string::npos)
	{
		mParameter = uri.substr(found + 1);
		uri.resize(found);
	}
	// filename parsing
	found = uri.find_last_of("/");
	if (found == std::string::npos)
	{
		throw 400;
	}
	if (found != uri.length() - 1) //  localhost/eee/fff
	{
		mURItype = Request::FILE;
		mFileName = uri.substr(found + 1);
		std::size_t	dot = uri.substr(found + 1).rfind(".");
		if (dot != std::string::npos)
		{
			std::string	extension = mFileName.substr(dot);
			// FIXME 하드코딩으로 리터럴을 넣어주었는데 config에 있는 확장자와 비교해야함 그런데 URI가 무엇인지 먼저 정해져야 config파일에서 특정 값을 가져올텐데...
			// 혹은 config파일에 값을 추가해야함 그것도 괜찮은 듯
			if (extension.compare(".bla") == 0 || extension.compare(".php") == 0)
			{
				mURItype = Request::CGI_PROGRAM;
			}
		}
		uri.resize(found);
	}
	// directory parsing
	mDirectory = uri;
	// * 일때 파일로 처리
	if (mMethod.compare("OPTIONS") == 0 &&
		(mFileName.rfind("*") != std::string::npos && mFileName.length() == 1))
	{
		mFileName.clear();
		mURItype = Request::DIRECTORY;
	}
	if (mMethod.compare("PUT") == 0)
	{
			mURItype = Request::FILE_TO_CREATE;
	}
	if (mMethod.compare("DELETE") == 0)
	{
			mURItype = Request::FILE_TO_DELETE;
	}
	// // ANCHOR URI parsing debug
	// if (mURItype == Request::DIRECTORY)
	// 	std::cout << "\t  mURItype : |DIRECTORY|"<< std::endl;
	// if (mURItype == Request::FILE)
	// 	std::cout << "\t  mURItype : |FILE|"<< std::endl;
	// if (mURItype == Request::FILE_TO_CREATE)
	// 	std::cout << "\t  mURItype : |FILE_TO_CREATE|"<< std::endl;
	// if (mURItype == Request::CGI_PROGRAM)
	// 	std::cout << "\t  mURItype : |CGI_PROGRAM|"<< std::endl;
	// std::cout << "\t    mQuery : |" << mQuery << "|" << std::endl;
	// std::cout << "\tmParameter : |" << mParameter << "|" << std::endl;
	// std::cout << "\t mFileName : |" << mFileName << "|" << std::endl;
	// std::cout << "\tmDirectory : |" << mDirectory << "|" << std::endl;
}

void								Request::SetURI(const std::string& uri)
{
	mURI = uri;
}

const std::string&					Request::GetURI(void) const
{
	return (mURI);
}

const std::string&					Request::GetDirectory(void) const
{
	return (mDirectory);
}

void								Request::SetDirectory(const std::string& directory)
{
	mDirectory = directory;
}

const std::string&					Request::GetFileName(void) const
{
	return (mFileName);
}

void								Request::SetFileName(const std::string& fileName)
{
	mFileName = fileName;
}

const std::string&					Request::GetParameter(void) const
{
	return (mParameter);
}

void								Request::SetParameter(const std::string& parameter)
{
	mParameter = parameter;
}

const std::string&					Request::GetQuery(void) const
{
	return (mQuery);
}

void								Request::SetQuery(const std::string& query)
{
	mQuery = query;
}

const Request::eURIType&			Request::GetURItype(void) const
{
	return (mURItype);
}

void								Request::SetURItype(const eURIType& uriType)
{
	mURItype = uriType;
}

const struct timeval&				Request::GetStartTime(void) const
{
	return (mStartTime);
}

void								Request::SetStartTime(const struct timeval& startTime)
{
	mStartTime = startTime;
}

void								Request::ShowMessage(void)
{
	// request line
	std::cout << GetMethod() << " " << GetURI() << " " << GetVersion() << std::endl;

	// request header
	for (std::map<std::string, std::string>::iterator it = GetHeaders().begin(); it != GetHeaders().end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	std::cout << std::endl;

	// body
	if (GetBody().empty() == false)
	{
		std::cout << GetBody() << std::endl;
	}
}

void									Request::ParseVersion(std::string& version)
{
	mVersion = version;
	std::size_t	found = version.find("/");
	std::string	scheme = version.substr(0, found);
	if (scheme.compare("HTTP") != 0)
	{
		throw 505;
	}
	version = version.substr(found + 1);
	found = version.find(".");
	if (found == std::string::npos)
	{
		throw 505;
	}
	version.erase(found, 1);
	int num = std::atoi(version.c_str());
	if (num > 11 || num <= 0)
	{
		throw 505;
	}
}