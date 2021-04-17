#pragma once
#include <vector>
#include "Config.hpp"
#include "Path.hpp"

/*
*Webserver
*>> Server
*>>>> Location
*>>>> Location
*>>>> Location
*>> Server
*>>>> Location
*>>>> Location
*>> Server
*>>>> Location
*>>>> Location
*>>>> Location
*	.
*	.
*	.
*/
class ConfigWebserver
{
	uint32_t							max_connection;
	std::vector<std::vector<Config> >	config_servers;		
	
	public:
								ConfigWebserver(const Path&);
};