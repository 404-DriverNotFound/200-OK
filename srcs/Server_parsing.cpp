#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	gEnv;

LocationPath::LocationPath() : mLocationPath(), mRoot(), mErrorPage("error.html"), mAutoIndex(false), mClientMaxBodySize(0), mAuthBasicUserFile()
{
	Path temp("index.html");
	this->mIndexPages.push_back(temp);

	this->mMethod.push_back("GET");
	this->mMethod.push_back("HEAD");
	this->mMethod.push_back("PUT");
	this->mMethod.push_back("POST");
	this->mMethod.push_back("DELETE");
	this->mMethod.push_back("OPTIONS");

	this->mCgiExtension.push_back(".bla");
}

LocationPath::~LocationPath()
{

}

LocationPath::LocationPath(const LocationPath &ref)
{
	*this = ref;
}

LocationPath&	LocationPath::operator=(const LocationPath &ref)
{
	if (this == &ref)
		return (*this);
	this->mErrorPage = ref.mErrorPage;
	this->mIndexPages = ref.mIndexPages;
	this->mLocationPath = ref.mLocationPath;
	this->mRoot = ref.mRoot;

	this->mAutoIndex = ref.mAutoIndex;
	this->mMethod = ref.mMethod;
	this->mCgiExtension = ref.mCgiExtension;
	this->mClientMaxBodySize = ref.mClientMaxBodySize;
	this->mAuthBasicUserFile = ref.mAuthBasicUserFile;
	return (*this);
}

serverBlock::serverBlock()
	: mlocationPaths()
	, mserverName("localhost")
	, mTimeOut(0)
{

}

serverBlock::~serverBlock()
{

}

serverBlock::serverBlock(const serverBlock &ref)
{
	*this = ref;
}

serverBlock&	serverBlock::operator=(const serverBlock &ref)
{
	if (this == &ref)
		return (*this);
	this->mlocationPaths = ref.mlocationPaths;
	this->mserverName = ref.mserverName;
	this->mTimeOut = ref.mTimeOut;
	return (*this);
}

Server::Server(void)
	: mManager(NULL)
	, mPort(8000)
	// , mServerBlocks()
	, mSocket(0)
	// , mPhase(READY)
{
}

Server::Server(ServerManager *serverManager)
	: mManager(serverManager)
	, mPort(8000)
	, mHost("0.0.0.0")
	, mSocket(0)
	// , mServerBlocks()
	// , mPhase(READY)
{
	
}

Server::~Server()
{
	
}

Server::Server(const Server &ref)
{
	*this = ref;
}

Server&	Server::operator=(const Server &ref)
{
	if (this == &ref)
		return (*this);
	this->mPort = ref.mPort;
	this->mServerBlocks = ref.mServerBlocks;
	this->mSocket = ref.mSocket;
	this->mHost = ref.mHost;
	this->mConnections = ref.mConnections;
	return (*this);
}


std::vector<serverBlock>::iterator Server::returnIteratorServerBlock(std::vector<serverBlock> &serverBlocks, std::string serverName)
{
	std::vector<serverBlock>::iterator it = serverBlocks.begin();
	while (it != serverBlocks.end())
	{
		if (serverName == it->mserverName)
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 serverBlock을 default로 잡음
	return (it);
}

std::vector<LocationPath>::iterator Server::returnIteratorLocationPath(std::vector<LocationPath> &locationPaths, std::string relative_str)
{
	std::vector<LocationPath>::iterator it = locationPaths.begin();
	Path relative_path(relative_str);
	while (it != locationPaths.end())
	{
		int i = 0;
		while (i < it->mLocationPath.getSize() && i < relative_path.getSize())
		{
			if (it->mLocationPath[i] != relative_path[i])
			{
				break ;
			}
			i++;
		}
		if (i == it->mLocationPath.getSize())
			return (it);
		it++;
		// if (relative_str == it->mLocationPath.getPath())
		// {
		// 	return (it);
		// }
	}
	it--; // NOTE 맨 뒤에 있는 locationPath을 default로 잡음
	return (it);
}

const int&	Server::getSocket(void) const{ return (this->mSocket); }

std::vector<serverBlock>&	Server::getServerBlocks(void){ return (this->mServerBlocks);}


