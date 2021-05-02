#include "Server.hpp"

LocationPath::LocationPath() : mlocationPath(), mroot(), merror_page("error.html")
{
	Path temp("index.html");
	this->mindex_pages.push_back(temp);
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
	return (*this);
}

ServerBlock::ServerBlock() : mserverName("localhost"), mlocationPaths()
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
	return (*this);
}

Server::Server() : mport(8000), mserverBlocks()
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
	return (*this);
}
