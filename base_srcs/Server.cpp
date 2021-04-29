#include "../base_headers/Server.hpp"

void Server::ShowServerConfigs()
{
	for (size_t i = 0; i < this->mconfig_locations.size(); i++)
	{
		cout << "----------------------"<< "Server: " << i <<"----------------------" << endl;
		cout << "server_name: " << this->mconfig_locations[i].mserver_name << endl;
		cout << "root: " << this->mconfig_locations[i].mroot.getPath() << endl;
		cout << "mlocation_path: " << this->mconfig_locations[i].mlocation_path.getPath() << endl;
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


// int Server::SetServer(std::vector<std::string> &gnl)
// {
// 	ServerConfigIdx configIdx;

// 	// ANCHOR Step1. index로 구역 나누기
// 	if (-1 == configIdx.Step1(configIdx, gnl)) // NOTE server block index로 나눔
// 		return (-1);
// 	if (-1 == configIdx.Step2(configIdx, gnl)) // NOTE location block index로 나눔
// 		return (-1);

// 	// STUB 출력: 서버 범위
// 	// for (size_t i = 0; i < configIdx.mserverNum; i++)
// 	// {
// 	// 	int start = 0; int end = 0;
// 	// 	Config default_config;
// 	// 	start = configIdx.mserverBracket[i].mstart + 1;
// 	// 	end = configIdx.mserverBracket[i].mend - 1;
// 	// 	while (start <= end)
// 	// 	{
// 	// 		cout << gnl[start] << endl;			
// 	// 		start++;
// 	// 	}
// 	// }
	
// 	// STUB 출력: location idx
// 	// cout << "Total serverNum: "<< configIdx.mserverNum << endl;
// 	// for (size_t i = 0; i < configIdx.mserverNum; i++)
// 	// {
// 	// 	cout << "location num: " << configIdx.mserverBracket[i].mlocationBlockNum << endl;
// 	// 	for (size_t j = 0; j < configIdx.mserverBracket[i].mlocationBlockNum; j++)
// 	// 	{
// 	// 		cout << "location start: " << configIdx.mserverBracket[i].mlocationBracket[j].mstart << endl;
// 	// 		cout << "location end  : " << configIdx.mserverBracket[i].mlocationBracket[j].mend << endl;			
// 	// 	}
// 	// 	cout << " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
// 	// }
// 	for (size_t i = 0; i < configIdx.mserverNum; i++)
// 	{
// 		configIdx.mtotalLocationNum += (configIdx.mserverBracket[i].mlocationBlockNum + 1); // NOTE location block 갯수에 default location을 더해야하기 때문에
// 	}
	
	
// 	// ANCHOR Step2. 값 넣기
// 	for (size_t i = 0; i < configIdx.mserverNum; i++)
// 	{
// 		Config default_location; // NOTE 하나의 서버가 될 것임
// 		std::string oneline;
// 		std::vector<std::string> split_vector;
// 		int start = configIdx.mserverBracket[i].mstart + 1;
// 		int end = configIdx.mserverBracket[i].mend - 1;
		
// 		// if (default_location.parsingServerBlock(gnl, start, end, configIdx.mserverBracket[i]) == -1)
// 		if (parsingServerBlock(gnl, default_location, start, end, configIdx.mserverBracket[i]) == -1)
// 		{
// 			cout << "parsingBlock error" << endl;
// 			return (-1);
// 		}

// 		this->mconfig_locations.push_back(default_location); // NOTE server의 default location

// 		for (size_t j = 0; j < configIdx.mserverBracket[i].mlocationBlockNum ; j++)
// 		{
// 			Config temp = default_location;
// 			int start2 = configIdx.mserverBracket[i].mlocationBracket[j].mstart + 1;
// 			int end2 = configIdx.mserverBracket[i].mlocationBracket[j].mend - 1;
// 			temp.mlocation_path = configIdx.mserverBracket[i].mlocationBracket[j].mlocation_path;
// 			// if (temp.parsingLocationBlock(gnl, start2, end2) == -1)
// 			if (parsingLocationBlock(gnl, temp, start2, end2) == -1)
// 			{
// 				cout << "parsingBlock error" << endl;
// 				return (-1);
// 			}
// 			this->mconfig_locations.push_back(temp); // NOTE 서버의 location 블록에 있는 정보를 넣는다.
// 		}
// 	}
// 	return (1);
// }