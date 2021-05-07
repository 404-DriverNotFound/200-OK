#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <string>

class Config
{
	public:
		Config(void);
		Config(std::string config_block, char** envp);

		Config&				operator=(const Config& other);

		const std::string&	get_m_software_name(void) const;
		const std::string&	get_m_softwrae_version(void) const;
		const std::string&	get_m_http_version(void) const;
		const std::string&	get_m_cgi_version(void) const;
		const char**		get_m_base_env(void) const;

	private:
		std::string			m_software_name;
		std::string			m_software_version;
		std::string			m_http_version;
		std::string			m_cgi_version;
		char**				m_base_env;
};

#endif