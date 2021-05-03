#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <vector>

class ServerManager
{
public:
	ServerManager(void);


	// SetType	// NOTE 잘 모르겠음

	const Config&	get_m_config(void) const;
	const int&		get_m_max_fd(void) const;
	void			set_m_config(const Config& config);
	void			set_m_max_fd(int fd);
	
	void			fdSet(fd, SetType);
	void			fdZero(SetType);
	void			fdClear(fd, SetType);
	void			fdIsset(fd, SetType);
	void			fdCopy(SetType);

	void			exitServer(void);
	void			createServer(void);
	void			runServer(void);

private:
	bool				splitConfigString(config_string, config_block, std::vector<server_string>);
	bool				splitServerString(server_string, server_block, std::vector<location_block>);
	bool				isValidConfigBlock(config_block);
	bool				isValidServerBlock(server_block);
	bool				isValidLocationBlock(location_block);


	std::vector<Server>	m_servers;
	Config				m_config;
	int					m_max_fd;
	fd_set				m_read_set;
	fd_set				m_read_copy_set;
	fd_set				m_write_set;
	fd_set				m_write_copy_set;
	fd_set				m_error_set;
	fd_set				m_error_copy_set;





};

#endif