#pragma once
# include "../../gnl/get_next_line_bonus.hpp"
// # include "../../srcs/Server/Socket.hpp"
# include "../Path/Path.hpp"
# include "../Utils/utils.hpp"
# include "../Server.hpp"
# include "../ConfigFile/ConfigFile.hpp"
# include <iostream>
# include <sys/types.h> // FIXME 확인해봐야함
# include <fcntl.h> // FIXME 확인해봐야함

# define START 0
# define END 1

class LocationBracket
{
public:
	int mstart;
	int mend;
	Path mlocation_path;
public:
	LocationBracket();
	virtual ~LocationBracket();
};

class ServerBracket
{
public:
	ServerBracket();
	virtual ~ServerBracket();
public:
	std::vector<LocationBracket> mlocationBracket;
	int mlocationBlockNum;
	// ANCHOR start = 0 & end = 0 이면, server Bracket 설정이 제대로 안된 상황
	int mstart; // start = 0
	int mend; // end = 0 
};

class ServerConfigIdx
{
public:
	ServerConfigIdx();
	virtual ~ServerConfigIdx();
	
	// NOTE Bracket{}과 개행을 기준으로 index로 구역을 나눌 때 사용하는 함수들
	int Step1(ServerConfigIdx &configs, std::vector<std::string> &gnl);
	int Step2(ServerConfigIdx &configs, std::vector<std::string> &gnl);
	
	bool check_bracket_syntax(std::vector<std::string> &gnl, int start, int end);
	void SetServerBracket(std::vector<std::string> &gnl, ServerConfigIdx &configs, int start);
	void SetLocationBracket(std::vector<std::string> &gnl, ServerConfigIdx &configs, int server_idx, int start, int end);

	// NOTE string 맨 앞에 있는 '\t' 을 삭제시켜줌
	void outdentTab(std::string &str);
public :
	std::vector<ServerBracket> mserverBracket;
	int mserverNum;
	int mtotalLocationNum;
};

int parsingServerBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end, ServerBracket &ServerBracket);
int parsingLocationBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end);
// int SetServer(Server &servers, std::vector<std::string> &gnl);
int isCorrespondedServerDirective(int index, ServerBracket &ServerBracket);