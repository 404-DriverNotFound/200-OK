#pragma once
#include "ConfigWebserver.hpp"
#include "Path.hpp"
#include "Server.hpp"
#include <sys/select.h>
#include <vector>

class Webserver
{
	ConfigWebserver			config;
	fd_set					to_be_checked;
	fd_set					to_be_checked_read;
	fd_set					to_be_checked_write;
	std::vector<Server>		servers;

	public:
					Webserver(const Path&); 	// config 파일의 경로를 받아서 초기화
	void			create_server(const std::vector<Config>& config_locations);
	void			start_server();				// 서버 시작

	private:
	void			parse_config(const Path& path_config);			// config 파일 해석
};