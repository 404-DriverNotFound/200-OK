#pragma once
#include <string>
#include "Path.hpp"

class Url
{
	std::string		scheme;
	std::string		user;
	std::string		pass;
	std::string		host;
	std::string		port;
	Path			path;
	std::string		query_string;
};