#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "Client.hpp"
#include "Config.hpp"
// #include "../yunslee_test/server_parsing/ServerConfigIdx.hpp"
// #include "../yunslee_test/all_header.hpp"

using namespace std;

class Server
{
public:
	std::vector<Config>		mconfig_locations;		// REVIEW '&' 삭제함. Server 객체를 만들어놓고 값을 집어넣고싶어서,
	// std::list<Client>	mclients;			// FIXME client class의 구성요소가 구현되지않아서 주석처리함.

public :
	Server(){};
	void ShowServerConfigs();
	
	// FIXME 헤더파일의 상호참조 문제를 해결해야함 #20
	// int SetServer(Server &servers, std::vector<std::string> &gnl);
};
