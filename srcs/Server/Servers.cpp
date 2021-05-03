#include "./Servers.hpp"

Servers::Servers() : mfdMax(-1) {};

Servers::~Servers(){};

int Servers::SetServers(Configs *configs)
{
	while (configs->mconfigs.size() != 0)
	{
		Config &config = configs->mconfigs[configs->mconfigs.size() - 1];
		int idxserver;
		if (-1 == (idxserver = this->GetIdxServer(config.mport)))
		{
			// NOTE port별로 서버도 없는 상황, 아예 새롭게 만들면 됨.
			Server server;
			server.mport = config.mport;
			
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			server.mserverBlocks.push_back(temp);

			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			server.mserverBlocks[0].mlocationPaths.push_back(temp2);

			this->mservers.push_back(server);
			configs->mconfigs.pop_back();
			continue;
		}
		Server &server = this->mservers[idxserver];
		int idxserverblock;
		if (-1 == (idxserverblock = this->GetIdxServerBlock(server.mserverBlocks, config.mserver_name)))
		{
			// NOTE port는 있으나 다른 서버네임을 가지고 있음
			ServerBlock temp;
			temp.mserverName = config.mserver_name;
			server.mserverBlocks.push_back(temp);
			
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			server.mserverBlocks[server.mserverBlocks.size() - 1].mlocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		ServerBlock &temp = server.mserverBlocks[idxserverblock];
		int idxlocationblock;
		if (-1 == (idxlocationblock = this->GetIdxLocationPath(temp.mlocationPaths, config.mlocation_path)))
		{
			// NOTE port도 있고, 서버네임도 있으나, location이 다른 경우
			LocationPath temp2;
			temp2.mlocationPath = config.mlocation_path;
			temp2.mroot = config.mroot;
			temp2.mindex_pages = config.mindex_pages;
			temp2.merror_page = config.merror_page;
			server.mserverBlocks[server.mserverBlocks.size() - 1].mlocationPaths.push_back(temp2);
			configs->mconfigs.pop_back();
			continue;
		}
		// NOTE 세개가 모두 같은 경우는 에러처리한다.
		configs->mconfigs.pop_back();
		return (-1);
	}
	return (1);
}

int Servers::GetIdxServer(int port)
{
	if (this->mservers.size() == 0)
		return (-1);
	for (size_t i = 0; i < mservers.size(); i++)
	{
		if (this->mservers[i].mport == port)
			return (i);
	}
	return (-1);
}

int Servers::GetIdxServerBlock(std::vector<ServerBlock> &serverBlocks, std::string serverName)
{
	if (serverBlocks.size() == 0)
		return (-1);
	for (size_t i = 0; i < serverBlocks.size(); i++)
	{
		ServerBlock temp = serverBlocks[i];
		if (temp.mserverName.compare(serverName) == 0)
			return (i);
	}
	return (-1);
}

int Servers::GetIdxLocationPath(std::vector<LocationPath> &locationPaths, Path locationPath)
{
	if (locationPaths.size() == 0)
		return (-1);
	for (size_t i = 0; i < locationPaths.size(); i++)
	{
		LocationPath &temp = locationPaths[i];
		if (temp.mlocationPath == locationPath)
			return (i);
	}
	return (-1);
}

int Servers::ShowServers()
{
	for (size_t i = 0; i < this->mservers.size(); i++)
	{
		Server &temp = this->mservers[i];
		std::cout << "port: " << temp.mport << endl;
		for (size_t j = 0; j < temp.mserverBlocks.size(); j++)
		{
			ServerBlock temp2 = temp.mserverBlocks[j];
			std::cout << "server_name: " << temp2.mserverName << endl;
			std::cout << temp2.mserverName << ": " << "locationPathsize: " << temp2.mlocationPaths.size() << endl;
			for (size_t k = 0; k < temp2.mlocationPaths.size(); k++)
			{
				LocationPath temp3 = temp2.mlocationPaths[k];
				std::cout << "mlocationpath: " << temp3.mlocationPath.getPath() << std::endl;
				std::cout << "error_page: " << temp3.merror_page.getPath() << std::endl;
				std::cout << "root: " << temp3.mroot.getPath() << std::endl;
				for (size_t l = 0; l < temp3.mindex_pages.size(); l++)
				{
					std::cout << "index_pages: " << temp3.mindex_pages[l].getPath() << std::endl;	/* code */
				}
				std::cout << "------------------------------ locationPath" << std::endl;
			}
			std::cout << "============================== server name" << std::endl;
		}
		std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- port" << std::endl;	
	}
	return (1);
}

int Servers::SetSockets()
{
	for (size_t i = 0; i < this->mservers.size(); i++)
	{
		Server &temp = this->mservers[i];
		temp.SetSocket();
	}
	return (1);
}

void Servers::SetFdMax()
{
	for (size_t i = 0; i < this->mservers.size(); i++)
	{
		Server &server =this->mservers[i];
		if (this->mfdMax < server.msocket.GetFd())
			this->mfdMax = server.msocket.GetFd();
	}
}

int Servers::GetFdMax()
{
	this->SetFdMax();
	return (this->mfdMax);
}