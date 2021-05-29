#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

LocationPath::LocationPath(void)
	: mLocationPath()
	, mRoot()
	, mErrorPage("error.html")
	, mAutoIndex(false)
	, mClientMaxBodySize(0)
{
	Path	temp("index.html");
	this->mIndexPages.push_back(temp);
	this->mMethods.push_back("GET");
	this->mMethods.push_back("HEAD");
	this->mMethods.push_back("PUT");
	this->mMethods.push_back("POST");
	this->mMethods.push_back("DELETE");
	this->mMethods.push_back("OPTIONS");
	this->mCgiExtensions.push_back(".bla");
}

LocationPath::~LocationPath(void)
{
}

LocationPath::LocationPath(const LocationPath& ref)
{
	*this = ref;
}

LocationPath&	LocationPath::operator=(const LocationPath& ref)
{
	if (this == &ref)
	{
		return (*this);
	}
	this->mErrorPage = ref.mErrorPage;
	this->mIndexPages = ref.mIndexPages;
	this->mLocationPath = ref.mLocationPath;
	this->mRoot = ref.mRoot;
	this->mAutoIndex = ref.mAutoIndex;
	this->mMethods = ref.mMethods;
	this->mCgiExtensions = ref.mCgiExtensions;
	this->mClientMaxBodySize = ref.mClientMaxBodySize;
	return (*this);
}

ServerBlock::ServerBlock()
	: mLocationPaths()
	, mServerName("localhost")
	, mTimeOut(0)
{
}

ServerBlock::~ServerBlock()
{
}

ServerBlock::ServerBlock(const ServerBlock& ref)
{
	*this = ref;
}

ServerBlock&	ServerBlock::operator=(const ServerBlock& ref)
{
	if (this == &ref)
		return (*this);
	this->mLocationPaths = ref.mLocationPaths;
	this->mServerName = ref.mServerName;
	this->mTimeOut = ref.mTimeOut;
	return (*this);
}

Server::Server(void)
	: mManager(NULL)
	, mPort(8000)
	, mSocket(0)
{
}

Server::Server(ServerManager* serverManager)
	: mManager(serverManager)
	, mPort(8000)
	, mSocket(0)
{
}

Server::~Server(void)
{
}

Server::Server(const Server& ref)
{
	*this = ref;
}

Server&	Server::operator=(const Server& ref)
{
	if (this == &ref)
	{
		return (*this);
	}
	this->mPort = ref.mPort;
	this->mServerBlocks = ref.mServerBlocks;
	this->mSocket = ref.mSocket;
	this->mConnections = ref.mConnections;
	return (*this);
}

std::vector<ServerBlock>::iterator Server::returnIteratorServerBlock(std::vector<ServerBlock>& serverBlocks, std::string hostName)
{
	std::vector<ServerBlock>::iterator it = serverBlocks.begin();
	while (it != serverBlocks.end())
	{
		if (hostName == it->mServerName)
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 serverBlock을 default로 잡음
	return (it);
}

std::vector<LocationPath>::iterator Server::returnIteratorLocationPath(std::vector<LocationPath>& locationPaths, std::string relativeStr)
{
	std::vector<LocationPath>::iterator it = locationPaths.begin();
	Path relativePath(relativeStr);
	while (it != locationPaths.end())
	{
		int i = 0;
		while (i < it->mLocationPath.getSize() && i < relativePath.getSize())
		{
			if (it->mLocationPath[i] != relativePath[i])
			{
				break ;
			}
			i++;
		}
		if (i == it->mLocationPath.getSize())
			return (it);
		it++;
		// if (relative_str == it->mlocationPath.getPath())
		// {
		// 	return (it);
		// }
	}
	it--; // NOTE 맨 뒤에 있는 locationPath을 default로 잡음
	return (it);
}

const int&	Server::getSocket(void) const
{
	return (this->mSocket);
}

std::vector<ServerBlock>&	Server::getServerBlocks(void)
{
	return (this->mServerBlocks);
}