#pragma once
# include "../all_header.hpp"

# include "../gnl/get_next_line_bonus.hpp"
# include <iostream>
# include <stdlib.h> // FIXME atoi 때문에 추가
# include <sys/types.h> // FIXME 확인해봐야함
# include <fcntl.h> // FIXME 확인해봐야함


# define START 0
# define END 1

class LocationBracket
{
	public:
		LocationBracket();
		virtual ~LocationBracket();
	public:
		int mstart;
		int mend;
		Path mlocation_path;
};

LocationBracket::LocationBracket() : mstart(0), mend(0), mlocation_path() {};
LocationBracket::~LocationBracket() {};

class ServerBracket
{
	public:
		std::vector<LocationBracket> mlocationBracket;
		int mlocationBlockNum;
		
		ServerBracket();
		virtual ~ServerBracket();
	public:
		// ANCHOR start = 0 & end = 0 이면, server Bracket 설정이 제대로 안된 상황
		int mstart; // start = 0
		int mend; // end = 0 
};

ServerBracket::ServerBracket() : mlocationBlockNum(0), mstart(0), mend(0){}
ServerBracket::~ServerBracket(){}


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

ServerConfigIdx::ServerConfigIdx() : mserverNum(0), mtotalLocationNum(0) {}
ServerConfigIdx::~ServerConfigIdx(){}

int parsingServerBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end, ServerBracket &ServerBracket);
int parsingLocationBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end);