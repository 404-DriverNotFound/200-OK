#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <set>	// FIXME 허용 헤더 아님
class Location
{
public:
	Location(location_block);

	const std::string&							get_m_uri(void) const;
	const std::string&							get_m_root_path(void) const;
	const std::set<std::string>&				get_m_allow_method(void) const;
	const std::string&							get_m_auth_basic_realm(void) const;
	const std::map<std::string, std::string>&	get_m_auth_basic_file(void) const;
	const std::set<std::string>&				get_m_index(void) const;
	const std::set<std::string>&				get_m_cgi(void) const;
	const bool&									get_m_autoindex(void) const;

private:
	std::string									m_uri;	// type 부정확 ??
	std::string									m_root_path;
	std::set<std::string>						m_allow_method;		//	FIXME set 허용 컨테이너 아님
	std::string									m_auth_basic_realm;
	std::map<std::string, std::string>			m_auth_basic_file;
	std::set<std::string>						m_index;			//	FIXME set 허용 컨테이너 아님
	std::set<std::string>						m_cgi;				//	FIXME set 허용 컨테이너 아님
	bool										m_autoindex;
};

#endif