#pragma once
#include "ConfigFile.hpp"
#include <iostream>
#include <vector>
#include <sys/types.h>
#include "../../gnl/get_next_line_bonus.hpp"
#include "../ServerConfigIdx/ServerConfigIdx.hpp"
#include <fcntl.h>
#include <unistd.h>

class ConfigFiles
{
public:
	void	ShowConfigs();
	int		ReadConfigFile(const char *fileName);
	int		SetGnl();
	int		SetConfigs();
	
	ConfigFiles();
	ConfigFiles(const char *fileName);
	virtual ~ConfigFiles();
	ConfigFiles(const ConfigFiles &);
	ConfigFiles&	operator=(const ConfigFiles &);
public:
	int							mConfigFd;
	std::vector<std::string>	mGnl;
	
	std::vector<ConfigFile>			mConfigs;
};
