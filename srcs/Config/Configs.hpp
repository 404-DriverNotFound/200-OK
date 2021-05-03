#pragma once
#include "Config.hpp"
#include <iostream>
#include <vector>
#include <sys/types.h>
#include "../../gnl/get_next_line_bonus.hpp"
#include "../ServerParsing/ServerConfigIdx.hpp"
#include <fcntl.h>
#include <unistd.h>

class Configs
{
public:
	void	ShowConfigs();
	int		ReadConfigFile(const char *fileName);
	int		SetGnl();
	int		SetConfigs();
	
	Configs();
	Configs(const char *fileName);
	virtual ~Configs();
	Configs(const Configs &);
	Configs&	operator=(const Configs &);
public:
	int							mconfigFd;
	std::vector<std::string>	mgnl;
	
	std::vector<Config>			mconfigs;
};
