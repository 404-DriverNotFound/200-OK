#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "../Config/Config.hpp"
#include "../Config/Configs.hpp"
#include "./Server.hpp"
// FIXME 아래 header 파일을 포함하는 순간, 컴파일이 안됨.
// #include "../ServerParsing/ServerConfigIdx.hpp"
#include <unistd.h>
// #include "../Client/Client.hpp"srcs/Server/Server.cpp
#include <sys/types.h>
#include <fcntl.h>
class Configs;

using namespace std;

class Servers
{
public:
	std::vector<Server>			mservers;
	
	// std::list<Client>	mclients;			// FIXME client class의 구성요소가 구현되지않아서 주석처리함.

public :
	Servers(){};
	int SetServers(Configs *configs);
	int GetIdxServer(int port);
	int GetIdxServerBlock(std::vector<ServerBlock> &serverBlocks, std::string serverName);
	int GetIdxLocationPath(std::vector<LocationPath> &locationPaths, Path locationPath);

	// FIXME 헤더파일의 상호참조 문제를 해결해야함 #20
	// int SetServer(Servers &servers, std::vector<std::string> &gnl);
};