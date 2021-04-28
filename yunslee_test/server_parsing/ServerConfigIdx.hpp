#pragma once
#include "../all_header.hpp"
#include "../gnl/get_next_line_bonus.hpp"
#include <stdlib.h> // FIXME atoi 때문에 추가
# include <sys/types.h> // FIXME 확인해봐야함
# include <fcntl.h> // FIXME 확인해봐야함


#define START 0
#define END 1

class locationBracket
{
	public:
		int start;
		int end;
		Config config;
		locationBracket();
		virtual ~locationBracket();
};

locationBracket::locationBracket() : start(0), end(0) {};
locationBracket::~locationBracket() {};

class serverBracket
{
	public:
		// ANCHOR start = 0 & end = 0 이면, server Bracket 설정이 제대로 안된 상황
		int start; // start = 0
		int end; // end = 0 
		std::vector<locationBracket> locationBracket;
		int locationBlockNum;
		
		serverBracket();
		virtual ~serverBracket();
};

serverBracket::serverBracket() : locationBlockNum(0), start(0), end(0){}
serverBracket::~serverBracket(){}


class ServerConfigIdx
{
	public :
		std::vector<serverBracket> mserverBracket;
		int serverNum;
		int totalLocationNum;
		ServerConfigIdx();
		virtual ~ServerConfigIdx();
		
		// NOTE 파싱하는데 필요한 함수들
		bool check_bracket_syntax(std::vector<std::string> &gnl, int start, int end);
		void SetServerBracket(std::vector<std::string> &gnl, ServerConfigIdx &configs, int start);
		void SetLocationBracket(std::vector<std::string> &gnl, ServerConfigIdx &configs, int server_idx, int start, int end);
		
		
		int Step1(ServerConfigIdx &configs, std::vector<std::string> &gnl);
		int Step2(ServerConfigIdx &configs, std::vector<std::string> &gnl);
		
		void outdentTab(std::string &str);
};

ServerConfigIdx::ServerConfigIdx() : serverNum(0), totalLocationNum(0) {}
ServerConfigIdx::~ServerConfigIdx(){}