#include "./ConfigFile.hpp"

ConfigFile::ConfigFile()
	: mserver_name("localhost")
	, mport(8000)
	, mtimeout(0)
	, mlocation_path()
	, mauto_index(false)
	, mroot()
	, mIndexPages()
	, mErrorPage()
	, mclient_max_body_size(0)
	, mhead_length_max(8000)
	, mbody_length_max(10000)
	, mauth()
{
	this->m_method.push_back("GET");
	this->m_method.push_back("HEAD");
	this->m_method.push_back("PUT");
	this->m_method.push_back("POST");
	this->m_method.push_back("DELETE");
	this->m_method.push_back("OPTIONS");

	this->mcgi_extension.push_back(".bla");
}

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
	this->mIndexPages = ref.mIndexPages;
	this->mErrorPage = ref.mErrorPage;
	this->mhead_length_max = ref.mhead_length_max;
	this->mbody_length_max = ref.mbody_length_max;
	this->mauth = ref.mauth;

	this->mauto_index = ref.mauto_index;
	this->mtimeout = ref.mtimeout;
	this->m_method = ref.m_method;
	this->mcgi_extension = ref.mcgi_extension;
	this->mclient_max_body_size = ref.mclient_max_body_size;
	return (*this);
}
