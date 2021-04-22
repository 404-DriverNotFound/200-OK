#pragma once
#include <string>
#include <vector>
#include <list>
#include "Client.hpp"
#include "Config.hpp"

class Server
{
	public:
		std::vector<Config>		config_locations; // & 레퍼런스 삭제함
		// std::list<Client>		clients;

	public :
		Server(){};
};
