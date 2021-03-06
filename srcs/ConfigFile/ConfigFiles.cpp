#include "ConfigFiles.hpp"

void ConfigFiles::ShowConfigs()
{
	for (size_t i = 0; i < this->mConfigs.size(); i++)
	{
		std::cout << "----------------------"<< "Server: " << i <<"----------------------" << std::endl;
		std::cout << "server_name: " << this->mConfigs[i].mServerName << std::endl;
		std::cout << "root: " << this->mConfigs[i].mRoot.getPath() << std::endl;
		std::cout << "location_path: " << this->mConfigs[i].mLocationPath.getPath() << std::endl;
		std::cout << "port: " << this->mConfigs[i].mPort << std::endl;
		std::cout << "auto_index: " << this->mConfigs[i].mAutoIndex << std::endl;
		std::cout << "timeout: " << this->mConfigs[i].mTimeOut << std::endl;
		std::cout << "client_max_body_size: " << this->mConfigs[i].mClientMaxBodySize << std::endl;
		std::cout << "host: " << this->mConfigs[i].mHost << std::endl;

		
		std::cout << "index_pages: ";
		for (size_t j = 0; j < this->mConfigs[i].mIndexPages.size(); j++)
		{
			std::cout << this->mConfigs[i].mIndexPages[j].getPath() << " ";
		}
		std::cout << std::endl;
		std::cout << "error_page: " << this->mConfigs[i].mErrorPage.getPath() << std::endl;
		std::cout << "auth_basic_user_file: " << this->mConfigs[i].mAuthBasicUserFile.getPath() << std::endl;

		std::cout << "method: ";
		for (size_t j = 0; j < this->mConfigs[i].mMethod.size(); j++)
		{
			std::cout << this->mConfigs[i].mMethod[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "cgi_program_path: ";
		for (size_t j = 0; j < this->mConfigs[i].mCgiProgramPath.size(); j++)
		{
			std::cout << this->mConfigs[i].mCgiProgramPath[j] << " ";
		}
		std::cout << std::endl;
		std::cout << "cgi_extension: ";
		for (size_t j = 0; j < this->mConfigs[i].mCgiExtension.size(); j++)
		{
			std::cout << this->mConfigs[i].mCgiExtension[j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "=========================================================" << std::endl;
}

int ConfigFiles::ReadConfigFile(const char *fileName)
{
	this->mConfigFd = open(fileName, O_RDONLY);
	if (this->mConfigFd < 0)
	{
		std::cout << ("fd open error") << std::endl;
		perror("ss");
		this->mConfigFd = -1;
		return (-1);
	}
	return (1);
}

int ConfigFiles::SetGnl()
{
	char *line;
	while (get_next_line(this->mConfigFd, &line) > 0)
	{
		this->mGnl.push_back(line);
		free(line);
	}
	this->mGnl.push_back(line);
	free(line);
	close(this->mConfigFd);
	return (1);
}

ConfigFiles::ConfigFiles()
{
	
}

ConfigFiles::ConfigFiles(const char *fileName)
{
	this->ReadConfigFile(fileName);
	this->SetGnl();
	this->SetConfigs();
}

ConfigFiles::~ConfigFiles()
{
	
}

ConfigFiles::ConfigFiles(const ConfigFiles &)
{
	
}

ConfigFiles&	ConfigFiles::operator=(const ConfigFiles &ref)
{
	if (this == &ref)
		return (*this);
	return (*this);	
}

int ConfigFiles::SetConfigs()
{
	ServerConfigIdx configIdx;

	// ANCHOR Step1. index??? ?????? ?????????
	if (-1 == configIdx.Step1(configIdx, this->mGnl)) // NOTE server block index??? ??????
		return (-1);
	if (-1 == configIdx.Step2(configIdx, this->mGnl)) // NOTE location block index??? ??????
		return (-1);

	for (int i = 0; i < configIdx.mServerNum; i++)
	{
		configIdx.mTotalLocationNum += (configIdx.mServerBracket[i].mLocationBlockNum + 1); // NOTE location block ????????? default location??? ??????????????? ?????????
	}
	
	// ANCHOR Step2. ??? ??????
	for (int i = 0; i < configIdx.mServerNum; i++)
	{
		ConfigFile default_location; // NOTE ????????? ????????? ??? ??????
		std::string oneline;
		std::vector<std::string> splitVector;
		int start = configIdx.mServerBracket[i].mStart + 1;
		int end = configIdx.mServerBracket[i].mEnd - 1;
		
		if (parsingserverBlock(this->mGnl, default_location, start, end, configIdx.mServerBracket[i]) == -1)
		{
			std::cout << "parsingBlock error" << std::endl;
			return (-1);
		}

		this->mConfigs.push_back(default_location); // NOTE (1)????????? default location??? ???????????? ?????????

		for (int j = 0; j < configIdx.mServerBracket[i].mLocationBlockNum ; j++)
		{
			ConfigFile temp = default_location;
			int start2 = configIdx.mServerBracket[i].mLocationBracket[j].mStart + 1;
			int end2 = configIdx.mServerBracket[i].mLocationBracket[j].mEnd - 1;
			temp.mLocationPath = configIdx.mServerBracket[i].mLocationBracket[j].mLocationPath;
			if (parsingLocationBlock(this->mGnl, temp, start2, end2) == -1)
			{
				std::cout << "parsingBlock error" << std::endl;
				return (-1);
			}
			this->mConfigs.push_back(temp); // NOTE (2)????????? location block ??? ????????? ?????????
		}
	}
	return (1);
}

