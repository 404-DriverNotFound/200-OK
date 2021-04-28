#include "../yunslee_test/all_header.hpp"

Config::Config() : server_name("localhost"), root(), port(80), index_pages(), error_page(), head_length_max(8000), location_path(),
					body_length_max(10000), autoindex(false), timeout(5), auth()
					{}
					
					
					
Config::~Config(){}


Config::Config(const Config &ref)
{
	this->operator=(ref);
}

Config &Config::operator=(const Config &ref)
{
	if (this == &ref)
		return (*this);
	this->server_name = ref.server_name;
	this->root = ref.root;
	this->location_path = ref.location_path;
	this->port = ref.port;
	this->index_pages = ref.index_pages;
	this->error_page = ref.error_page;
	this->head_length_max = ref.head_length_max;
	this->body_length_max = ref.body_length_max;
	this->autoindex = ref.autoindex;
	this->timeout = ref.timeout;
	this->auth = ref.auth;
	return (*this);
}
