#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>

class Config
{
public:
	Config(std::string config_block, char** envp); // REVIEW: config_block 은 string 으로 들어오는 것 확실하다. (ServerManager.cpp 에서 참고하면 된다)

	const std::string&	get_m_software_name(void) const;
	const std::string&	get_m_softwrae_version(void) const;
	const std::string&	get_m_http_version(void) const;
	const std::string&	get_m_cgi_version(void) const;
	const char**		get_m_base_env(void) const;

private:
	std::string			m_software_name; // NOTE: 웹서버 소프트웨어 이름
	std::string			m_software_version; // NOTE: 웹서버 소프트웨어 버전
	std::string			m_http_version; // NOTE: HTTP 프로토콜 버전
	std::string			m_cgi_version; // NOTE: cgi 버전
	char**				m_base_env;
};

#endif