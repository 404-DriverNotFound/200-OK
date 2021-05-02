#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "../Config/Config.hpp"
// #include "../Client/Client.hpp"

// FIXME 아래 header 파일을 포함하는 순간, 컴파일이 안됨.
// #include "../ServerParsing/ServerConfigIdx.hpp"
#include "unistd.h"
#include "sys/types.h"
#include "fcntl.h"

using namespace std;

class Server
{
public:
	std::vector<Config>		mconfig_locations;		// REVIEW '&' 삭제함. Server 객체를 만들어놓고 값을 집어넣고싶어서,
	// std::list<Client>	mclients;			// FIXME client class의 구성요소가 구현되지않아서 주석처리함.
	std::vector<std::string> gnl;
	int config_fd;

public :
	Server(){};
	void ShowServerConfigs();
	int ReadConfigFile(const char *fileName);
	int SetGnl(int fd);
	
	// FIXME 헤더파일의 상호참조 문제를 해결해야함 #20
	// int SetServer(Server &servers, std::vector<std::string> &gnl);
};
