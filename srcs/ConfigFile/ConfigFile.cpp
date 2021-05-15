#include "./ConfigFile.hpp"

ConfigFile::ConfigFile() : mserver_name("localhost"), mroot(), mport(8000), mindex_pages(), merror_page(), mhead_length_max(8000), mlocation_path(),
					mbody_length_max(10000), mauth(), mauto_index(false), mtimeout(0)
					{}

ConfigFile::~ConfigFile(){}

ConfigFile::ConfigFile(const ConfigFile &ref)
{
	this->operator=(ref);
}

ConfigFile &ConfigFile::operator=(const ConfigFile &ref)
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
	this->mauth = ref.mauth;

	this->mauto_index = ref.mauto_index;
	this->mtimeout = ref.mtimeout;
	return (*this);
}
