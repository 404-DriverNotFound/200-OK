#pragma once
#include <string>
#include <vector>
#include <list>
#include "Client.hpp"
#include "Config.hpp"

class Server
{
	std::vector<Config>&	config_locations;		
	std::list<Client>		clients;
};