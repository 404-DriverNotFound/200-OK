#include "Server.hpp"
#include "ServerManager.hpp" // NOTE 상호참조 문제를 해결하기 위해서!
#include "Response.hpp"

extern char**	g_env;

LocationPath::LocationPath() : mlocationPath(), mroot(), merror_page("error.html"), mauto_index(false), mclient_max_body_size(0), mauth_basic_user_file()
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

	this->mauto_index = ref.mauto_index;
	this->m_method = ref.m_method;
	this->mcgi_extension = ref.mcgi_extension;
	this->mclient_max_body_size = ref.mclient_max_body_size;
	this->mauth_basic_user_file = ref.mauth_basic_user_file;
	return (*this);
}

ServerBlock::ServerBlock()
	: mlocationPaths()
	, mserverName("localhost")
	, mtimeout(0)
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
	return (*this);
}

Server::Server(void)
	: m_manager(NULL)
	, mport(8000)
	// , mserverBlocks()
	, msocket(0)
	// , mPhase(READY)
{
}

Server::Server(ServerManager *serverManager)
	: m_manager(serverManager)
	, mport(8000)
	, mhost("0.0.0.0")
	, msocket(0)
	// , mserverBlocks()
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
	this->mhost = ref.mhost;
	this->m_connections = ref.m_connections;
	return (*this);
}


std::vector<ServerBlock>::iterator Server::return_iterator_serverblock(std::vector<ServerBlock> &serverblocks, std::string servername)
{
	std::vector<ServerBlock>::iterator it = serverblocks.begin();
	while (it != serverblocks.end())
	{
		if (servername == it->mserverName)
		{
			return (it);
		}
		it++;
	}
	it--; // NOTE 맨 뒤에 있는 serverBlock을 default로 잡음
	return (it);
}

std::vector<LocationPath>::iterator Server::return_iterator_locationpathlocationPath(std::vector<LocationPath> &locationpaths, std::string relative_str)
{
	std::vector<LocationPath>::iterator it = locationpaths.begin();
	Path relative_path(relative_str);
	while (it != locationpaths.end())
	{
		int i = 0;
		while (i < it->mlocationPath.getSize() && i < relative_path.getSize())
		{
			if (it->mlocationPath[i] != relative_path[i])
			{
				break ;
			}
			i++;
		}
		if (i == it->mlocationPath.getSize())
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

const int&	Server::get_m_fd(void) const{ return (this->msocket); }

std::vector<ServerBlock>&	Server::get_m_serverBlocks(void){ return (this->mserverBlocks);}


