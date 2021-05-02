#include "Configs.hpp"

void Configs::ShowConfigs()
{
	for (size_t i = 0; i < this->mconfigs.size(); i++)
	{
		std::cout << "----------------------"<< "Server: " << i <<"----------------------" << std::endl;
		std::cout << "server_name: " << this->mconfigs[i].mserver_name << std::endl;
		std::cout << "root: " << this->mconfigs[i].mroot.getPath() << std::endl;
		std::cout << "location_path: " << this->mconfigs[i].mlocation_path.getPath() << std::endl;
		std::cout << "port: " << this->mconfigs[i].mport << std::endl;
		
		std::cout << "index_pages: ";
		for (size_t j = 0; j < this->mconfigs[i].mindex_pages.size(); j++)
		{
			std::cout << this->mconfigs[i].mindex_pages[j].getPath() << " ";
		}
		std::cout << std::endl;
		std::cout << "error_page: " << this->mconfigs[i].merror_page.getPath() << std::endl;
	}
	std::cout << "=========================================================" << std::endl;
}

int Configs::ReadConfigFile(const char *fileName)
{
	char buffer[BUFSIZ];
	this->config_fd = open(fileName, O_RDWR);
	if (this->config_fd < 0)
	{
		std::cout << ("fd open error") << std::endl;
		this->config_fd = -1;
		return (-1);
	}
	return (1);
}

int Configs::SetGnl()
{
	char *line;
	while (get_next_line(this->config_fd, &line) > 0)
	{
		this->mgnl.push_back(line);
		free(line);
	}
	this->mgnl.push_back(line);
	free(line);
	close(this->config_fd);
	return (1);
}

Configs::Configs()
{
	
}

Configs::Configs(const char *fileName)
{
	this->ReadConfigFile(fileName);
	this->SetGnl();
	this->SetConfigs();
}

Configs::~Configs()
{
	
}

Configs::Configs(const Configs &)
{
	
}

Configs&	Configs::operator=(const Configs &ref)
{
	if (this == &ref)
		return (*this);
	return (*this);	
}


int Configs::SetConfigs()
{
	ServerConfigIdx configIdx;

	// ANCHOR Step1. index로 구역 나누기
	if (-1 == configIdx.Step1(configIdx, this->mgnl)) // NOTE server block index로 나눔
		return (-1);
	if (-1 == configIdx.Step2(configIdx, this->mgnl)) // NOTE location block index로 나눔
		return (-1);

	for (size_t i = 0; i < configIdx.mserverNum; i++)
	{
		configIdx.mtotalLocationNum += (configIdx.mserverBracket[i].mlocationBlockNum + 1); // NOTE location block 갯수에 default location을 더해야하기 때문에
	}
	
	// ANCHOR Step2. 값 넣기
	for (size_t i = 0; i < configIdx.mserverNum; i++)
	{
		Config default_location; // NOTE 하나의 서버가 될 것임
		std::string oneline;
		std::vector<std::string> split_vector;
		int start = configIdx.mserverBracket[i].mstart + 1;
		int end = configIdx.mserverBracket[i].mend - 1;
		
		if (parsingServerBlock(this->mgnl, default_location, start, end, configIdx.mserverBracket[i]) == -1)
		{
			std::cout << "parsingBlock error" << std::endl;
			return (-1);
		}

		this->mconfigs.push_back(default_location); // NOTE (1)서버의 default location을 기본으로 넣어줌

		for (size_t j = 0; j < configIdx.mserverBracket[i].mlocationBlockNum ; j++)
		{
			Config temp = default_location;
			int start2 = configIdx.mserverBracket[i].mlocationBracket[j].mstart + 1;
			int end2 = configIdx.mserverBracket[i].mlocationBracket[j].mend - 1;
			temp.mlocation_path = configIdx.mserverBracket[i].mlocationBracket[j].mlocation_path;
			if (parsingLocationBlock(this->mgnl, temp, start2, end2) == -1)
			{
				std::cout << "parsingBlock error" << std::endl;
				return (-1);
			}
			this->mconfigs.push_back(temp); // NOTE (2)서버의 location block 이 있다면 넣어줌
		}
	}
	return (1);
}

