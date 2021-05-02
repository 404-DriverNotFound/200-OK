#pragma once
#include "../Path/Path.hpp"
#include <iostream>
#include <vector>

class LocationPath
{
public:
	Path				mlocationPath;		// def = 
	Path				mroot;				// def = 
	std::vector<Path>	mindex_pages;		// def = index.html
	Path				merror_page;		// def = error.html

public:
	LocationPath();
	virtual ~LocationPath();
	LocationPath(const LocationPath &);
	LocationPath&	operator=(const LocationPath &);
};

class ServerBlock
{
public :
	std::vector<LocationPath>	mlocationPaths;
	std::string					mserverName;

public :
	ServerBlock();
	virtual ~ServerBlock();
	ServerBlock(const ServerBlock &);
	ServerBlock&	operator=(const ServerBlock &);
};

class Server
{
public:
	Server();
	virtual ~Server();
	Server(const Server &);
	Server&	operator=(const Server &);

public :
	uint16_t					mport; // def = 80;
	std::vector<ServerBlock>	mserverBlocks;
};