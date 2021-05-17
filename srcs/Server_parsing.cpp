#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

LocationPath::LocationPath() : mlocationPath(), mroot(), merror_page("error.html")
{
	Path temp("index.html");
	this->mindex_pages.push_back(temp);

	this->m_method.push_back("GET");
	this->m_method.push_back("HEAD");
	this->m_method.push_back("PUT");
	this->m_method.push_back("POST");
	this->m_method.push_back("DELETE");
	this->m_method.push_back("OPTIONS");

	this->mcgi_extension.push_back(".bla");
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
	this->merror_page = ref.merror_page;
	this->mindex_pages = ref.mindex_pages;
	this->mlocationPath = ref.mlocationPath;
	this->mroot = ref.mroot;

	this->m_method = ref.m_method;
	this->mcgi_extension = ref.mcgi_extension;
	return (*this);
}

ServerBlock::ServerBlock() : mserverName("localhost"), mlocationPaths(), mauto_index(false), mtimeout(0)
{

}

ServerBlock::~ServerBlock()
{

}

ServerBlock::ServerBlock(const ServerBlock &ref)
{
	*this = ref;
}

ServerBlock&	ServerBlock::operator=(const ServerBlock &ref)
{
	if (this == &ref)
		return (*this);
	this->mlocationPaths = ref.mlocationPaths;
	this->mserverName = ref.mserverName;
	this->mtimeout = ref.mtimeout;
	this->mauto_index = ref.mauto_index;
	return (*this);
}

Server::Server(void)
	: mport(8000)
	// , mserverBlocks()
	, m_manager(NULL)
	, msocket(0)
	// , mPhase(READY)
{
}

Server::Server(ServerManager *serverManager)
	: mport(8000)
	// , mserverBlocks()
	, m_manager(serverManager)
	, msocket(0)
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
	this->mport = ref.mport;
	this->mserverBlocks = ref.mserverBlocks;
	this->msocket = ref.msocket;
	this->m_connections = ref.m_connections;
	return (*this);
}


std::vector<ServerBlock>::iterator Server::return_iterator_serverblock(std::vector<ServerBlock> &serverblocks, std::string hostname)
{
	std::vector<ServerBlock>::iterator it = serverblocks.begin();
	while (it != serverblocks.end())
	{
		if (hostname == it->mserverName)
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 serverBlock을 default로 잡음
	return (it);
}

std::vector<LocationPath>::iterator Server::return_iterator_locationpathlocationPath(std::vector<LocationPath> &locationpaths, std::string locationpath_str)
{
	std::vector<LocationPath>::iterator it = locationpaths.begin();
	while (it != locationpaths.end())
	{
		if (locationpath_str == it->mlocationPath.getPath())
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 locationPath을 default로 잡음
	return (it);
}

const int&	Server::get_m_fd(void) const{ return (this->msocket); }

std::vector<ServerBlock>&	Server::get_m_serverBlocks(void){ return (this->mserverBlocks);}


