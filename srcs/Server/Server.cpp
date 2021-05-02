#include "./Server.hpp"

void Server::ShowServerConfigs()
{
	for (size_t i = 0; i < this->mconfig_locations.size(); i++)
	{
		cout << "----------------------"<< "Server: " << i <<"----------------------" << endl;
		cout << "server_name: " << this->mconfig_locations[i].mserver_name << endl;
		cout << "root: " << this->mconfig_locations[i].mroot.getPath() << endl;
		cout << "location_path: " << this->mconfig_locations[i].mlocation_path.getPath() << endl;
		cout << "port: " << this->mconfig_locations[i].mport << endl;
		
		cout << "index_pages: ";
		for (size_t j = 0; j < this->mconfig_locations[i].mindex_pages.size(); j++)
		{
			cout << this->mconfig_locations[i].mindex_pages[j].getPath() << " ";
		}
		cout << endl;
		cout << "error_page: " << this->mconfig_locations[i].merror_page.getPath() << endl;
	}
	cout << "=========================================================" << endl;
}

int Server::ReadConfigFile(const char *fileName)
{
	char buffer[BUFSIZ];
	this->config_fd = open(fileName, O_RDWR);
	if (this->config_fd < 0)
	{
		cout << ("fd open error") << endl;
		this->config_fd = -1;
		return (-1);
	}
	return (1);
}

int Server::SetGnl(int fd_conf)
{
	char *line;
	while (get_next_line(fd_conf, &line) > 0)
	{
		this->gnl.push_back(line);
		free(line);
	}
	this->gnl.push_back(line);
	free(line);
	return (1);
}
