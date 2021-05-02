#pragma once
#include "../Path/Path.hpp"
#include <iostream>
#include <vector>

class LocationPath
{
public:
	Path				mlocationPath;		// def = 

	// NOTE - location에서 덮어씌워지는 요소들
	Path				mroot;				// def = 
	std::vector<Path>	mindex_pages;		// def = index.html
	Path				merror_page;			// def = error.html
};

class ServerBlock
{
public :
	std::vector<LocationPath>	mlocationPaths;
	std::string					mserverName;
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