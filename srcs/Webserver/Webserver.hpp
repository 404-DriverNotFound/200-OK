#pragma once
#include "../Server/Socket.hpp"
#include "../Server/Servers.hpp"
#include "../HttpMessage/HttpMessageRequest.hpp"
#include "../HttpMessage/HttpMessageResponse.hpp"
#include <sys/select.h>
#include <iostream>
#include <vector>
// #include "ConfigWebserver.hpp"
// #include "Path.hpp"

class Webserver
{
public:
	// ConfigWebserver			config;
	// fd_set					to_be_checked;
	// fd_set					to_be_checked_read;
	// fd_set					to_be_checked_write;
	Servers		servers;

public:
	Webserver(int argc, char** argv, char** envp);
	void			start_server(void);				// 서버 시작
					// Webserver(const Path&); 	// config 파일의 경로를 받아서 초기화
	// void			create_server(const std::vector<Config>& config_locations);

	// private:
	// void			parse_config(const Path& path_config);			// config 파일 해석
};