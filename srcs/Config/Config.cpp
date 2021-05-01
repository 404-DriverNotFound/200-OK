#include "./Config.hpp"

Config::Config() : mserver_name("localhost"), mroot(), mport(80), mindex_pages(), merror_page(), mhead_length_max(8000), mlocation_path(),
					mbody_length_max(10000), mautoindex(false), mtimeout(5), mauth()
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
	this->mserver_name = ref.mserver_name;
	this->mroot = ref.mroot;
	this->mlocation_path = ref.mlocation_path;
	this->mport = ref.mport;
	this->mindex_pages = ref.mindex_pages;
	this->merror_page = ref.merror_page;
	this->mhead_length_max = ref.mhead_length_max;
	this->mbody_length_max = ref.mbody_length_max;
	this->mautoindex = ref.mautoindex;
	this->mtimeout = ref.mtimeout;
	this->mauth = ref.mauth;
	return (*this);
}
