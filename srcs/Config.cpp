#include "Config.hpp"

Config::Config(void)
{
}

Config::Config(std::string config_block, char** envp)
{
	
}

Config&			Config::operator=(const Config& other)
{
	m_software_name = other.m_software_name;
	m_software_version = other.m_software_version;
	m_http_version = other.m_http_version;
	m_cgi_version = other.m_cgi_version;
	m_base_env = other.m_base_env;
	return (*this);
}