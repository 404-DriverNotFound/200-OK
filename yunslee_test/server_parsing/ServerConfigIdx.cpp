#include "./ServerConfigIdx.hpp"

using namespace std;

void ServerConfigIdx::outdentTab(std::string &str)
{
	if (str.size() < 2 || str[0] != '\t')
		return ;
	std::string temp;
	temp.clear();
	for (size_t i = 1; i < str.size(); i++)
	{
		temp.append(1, str[i]);
	}
	str = temp;
	return ;
}

bool ServerConfigIdx::check_bracket_syntax(std::vector<std::string> &gnl, int start, int end)
{
	int i = start;
	int startCheck = false;
	int endCheck = false;
	while (i <= end)
	{
		if (gnl[i].size() == 1 && gnl[i].compare("{") == 0)
			startCheck++;
		else if (gnl[i].size() == 1 && gnl[i].compare("}") == 0)
			endCheck++;
		i++;
		if (startCheck == true && endCheck == true) // NOTE true 로 탈출하는 조건
			return (true);
		else if (startCheck == false && endCheck == true) // } 로 먼저 시작한 경우
			return (false);
		else if (startCheck > true && endCheck == false)
			return (false);
	}
	return (false); // NOTE false 반복문에서 true로 탈출하지 못하면, 무조건 false임	
}

void ServerConfigIdx::SetServerBracket(std::vector<std::string> &gnl, ServerConfigIdx &configidx, int start)
{
	int i = start;
	bool startCheck = false;
	bool endCheck = false;
	ServerBracket temp;
	// int ServerBracket[2]; ServerBracket[START] = 0; ServerBracket[END] = 0;
	while (i < gnl.size())
	{
		if (gnl[i].size() == 1 && gnl[i].compare("{") == 0)
		{
			// cout << gnl[i] << "; "<< gnl[i].size() << endl;
			temp.mstart = i;
			startCheck = true;
		}
		else if (gnl[i].size() == 1 && gnl[i].compare("}") == 0)
		{
			// cout << gnl[i] << "; "<< gnl[i].size() << endl;
			temp.mend = i;
			endCheck = true;
		}
		i++;
		if (startCheck == true && endCheck == true)
		{
			configidx.mserverNum++;
			configidx.mserverBracket.push_back(temp);
			return;
		}
	}
}

void ServerConfigIdx::SetLocationBracket(std::vector<std::string> &gnl, ServerConfigIdx &configidx, int server_idx, int start, int end)
{
	// error 처리는 잘 안함
	bool startCheck = false;
	bool endCheck = false;
	LocationBracket temp;
	while (start <= end)
	{
		// cout << gnl[start] << endl;
		if (gnl[start].size() == 1 && gnl[start].compare("{") == 0)
		{
			temp.mstart = start;
			startCheck = true;
		}
		else if (gnl[start].size() == 1 && gnl[start].compare("}") == 0)
		{
			temp.mend = start;
			endCheck = true;
		}
		if (startCheck == true && endCheck == true)
		{
			configidx.mserverBracket[server_idx].mlocationBlockNum++;
			configidx.mserverBracket[server_idx].mlocationBracket.push_back(temp);
			return ;
		}
		if (gnl[start].size() != 1)
			outdentTab(gnl[start]);
		start++;
	}
}

// serverConfig를 채워넣음 파싱 첫 단계
int ServerConfigIdx::Step1(ServerConfigIdx &configidx, std::vector<std::string> &gnl)
{
	int i = 0;
	int j = 0;
	while (i < gnl.size())
	{
		if (gnl[i].compare("server") == 0) // server block 단위 하나 처리
		{
			if (configidx.check_bracket_syntax(gnl, i + 1, gnl.size()) == true)
			{
				configidx.SetServerBracket(gnl, configidx, i + 1);
				i = configidx.mserverBracket[configidx.mserverNum - 1].mend;
			}
		}
		i++;
	}
}

int ServerConfigIdx::Step2(ServerConfigIdx &configidx, std::vector<std::string> &gnl)
{
	for (int serverNum = 0; serverNum < configidx.mserverNum; serverNum++)
	{
		int start = configidx.mserverBracket[serverNum].mstart + 1;
		int end = configidx.mserverBracket[serverNum].mend - 1;
		int temp = start;
		configidx.mserverBracket[serverNum].mlocationBlockNum = 0;
		while (temp <= end)
			configidx.outdentTab(gnl[temp++]);
		while (start <= end)
		{
			if (gnl[start].find("location ") != std::string::npos) // server block 단위 하나 처리
			{
				std::vector<std::string> temp;
				ft_split_vector(temp, gnl[start], " ");
				std::string mlocation_path = temp[1];
				if (configidx.check_bracket_syntax(gnl, start + 1, end) == true) // NOTE { 로 시작하는 라인을 index(=start+1)로 넘겨줌
				{
					
					configidx.SetLocationBracket(gnl, configidx, serverNum, start, end);
					int location_idx = configidx.mserverBracket[serverNum].mlocationBlockNum - 1;
					configidx.mserverBracket[serverNum].mlocationBracket[location_idx].mlocation_path = mlocation_path;
					// cout << "mlocation_path: " << configidx.mserverBracket[serverNum].mlocationBracket[location_idx].mlocation_path.getPath() << endl;
					start = configidx.mserverBracket[serverNum].mlocationBracket[location_idx].mend; // NOTE location '}' 이후에 바로 다음줄에 location이 나오는 경우 때문에 '+1'을 해주지 않음
				}
			}
			start++;
		}
	}
}

int parsingServerBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end, ServerBracket &ServerBracket)
{
	std::string oneline;
	std::vector<std::string> split_vector;
	bool avoidLocationBlock = false;
	while (start <= end)
	{
		avoidLocationBlock = false;
		for (size_t i = 0; i < ServerBracket.mlocationBracket.size(); i++)
		{
			if (start >= ServerBracket.mlocationBracket[i].mstart && start <= ServerBracket.mlocationBracket[i].mend)
			{
				avoidLocationBlock = true;
				break;
			}
		}
		if (avoidLocationBlock == true)
		{
			start++;
			continue;
		}
		oneline = gnl[start];
		split_vector.clear();
		ft_split_vector(split_vector, oneline, " ");
		//
		
		// void function(std::vector<std::string> split_vector, Config &default_location);
		
		// STUB if문 비교하기
		if (split_vector[0].compare("server_name") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.mserver_name = split_vector[1];
		}
		else if (split_vector[0].compare("root") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path root(split_vector[1]);
			default_location.mroot = root;
		}
		else if (split_vector[0].compare("port") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.mport = atoi(split_vector[1].c_str());
		}
		else if (split_vector[0].compare("index_pages") == 0)
		{
			int i = 1;
			std::vector<Path> index_pages;
			while (i < split_vector.size())
			{
				Path index_page(split_vector[i]);
				index_pages.push_back(index_page);
				i++;
			}
			default_location.mindex_pages = index_pages;
		}
		else if (split_vector[0].compare("error_page") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path error_page(split_vector[1]);
			default_location.merror_page = error_page;
		}
		start++;
	}
	return (1);
}

int parsingLocationBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end)
{
	std::string oneline;
	std::vector<std::string> split_vector;
	bool exist_index_pages = false;
	
	while (start <= end)
	{
		oneline = gnl[start];
		split_vector.clear();
		ft_split_vector(split_vector, oneline, " ");
		//
		
		// void function(std::vector<std::string> split_vector, Config &default_location);
		
		// STUB if문 비교하기
		if (split_vector[0].compare("server_name") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.mserver_name = split_vector[1];
		}
		else if (split_vector[0].compare("root") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path root(split_vector[1]);
			default_location.mroot = root;
		}
		else if (split_vector[0].compare("port") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.mport = atoi(split_vector[1].c_str());
		}
		else if (split_vector[0].compare("index_pages") == 0)
		{
			int i = 1;
			std::vector<Path> index_pages;
			while (i < split_vector.size())
			{
				
				Path index_page(split_vector[i]);
				if (exist_index_pages == true)
					default_location.mindex_pages.push_back(split_vector[i]);
				else
					index_pages.push_back(split_vector[i]);
				i++;
			}
			if (exist_index_pages == false)
			{
				default_location.mindex_pages = index_pages;
				exist_index_pages = true;
			}
		}
		else if (split_vector[0].compare("error_page") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path error_page(split_vector[1]);
			default_location.merror_page = error_page;
		}
		start++;
	}
	return (1);
}

int SetServer(Server &servers, std::vector<std::string> &gnl)
{
	ServerConfigIdx configIdx;

	// ANCHOR Step1. index로 구역 나누기
	if (-1 == configIdx.Step1(configIdx, gnl)) // NOTE server block index로 나눔
		return (-1);
	if (-1 == configIdx.Step2(configIdx, gnl)) // NOTE location block index로 나눔
		return (-1);

	// STUB 출력: 서버 범위
	// for (size_t i = 0; i < configIdx.mserverNum; i++)
	// {
	// 	int start = 0; int end = 0;
	// 	Config default_config;
	// 	start = configIdx.mserverBracket[i].mstart + 1;
	// 	end = configIdx.mserverBracket[i].mend - 1;
	// 	while (start <= end)
	// 	{
	// 		cout << gnl[start] << endl;			
	// 		start++;
	// 	}
	// }
	
	// STUB 출력: location idx
	// cout << "Total serverNum: "<< configIdx.mserverNum << endl;
	// for (size_t i = 0; i < configIdx.mserverNum; i++)
	// {
	// 	cout << "location num: " << configIdx.mserverBracket[i].mlocationBlockNum << endl;
	// 	for (size_t j = 0; j < configIdx.mserverBracket[i].mlocationBlockNum; j++)
	// 	{
	// 		cout << "location start: " << configIdx.mserverBracket[i].mlocationBracket[j].mstart << endl;
	// 		cout << "location end  : " << configIdx.mserverBracket[i].mlocationBracket[j].mend << endl;			
	// 	}
	// 	cout << " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
	// }
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
		
		// if (default_location.parsingServerBlock(gnl, start, end, configIdx.mserverBracket[i]) == -1)
		if (parsingServerBlock(gnl, default_location, start, end, configIdx.mserverBracket[i]) == -1)
		{
			cout << "parsingBlock error" << endl;
			return (-1);
		}

		servers.mconfig_locations.push_back(default_location); // NOTE server의 default location

		for (size_t j = 0; j < configIdx.mserverBracket[i].mlocationBlockNum ; j++)
		{
			Config temp = default_location;
			int start2 = configIdx.mserverBracket[i].mlocationBracket[j].mstart + 1;
			int end2 = configIdx.mserverBracket[i].mlocationBracket[j].mend - 1;
			temp.mlocation_path = configIdx.mserverBracket[i].mlocationBracket[j].mlocation_path;
			// if (temp.parsingLocationBlock(gnl, start2, end2) == -1)
			if (parsingLocationBlock(gnl, temp, start2, end2) == -1)
			{
				cout << "parsingBlock error" << endl;
				return (-1);
			}
			servers.mconfig_locations.push_back(temp); // NOTE 서버의 location 블록에 있는 정보를 넣는다.
		}
	}
	return (1);
}

int main()
{
	Server server;
	int fd_conf;
	char buffer[BUFSIZ];
	fd_conf = open("/Users/yunslee/webserv_200/yunslee_test/server_parsing/server_config3", O_RDWR);
	if (fd_conf < 0)
	{
		cout << ("fd open error") << endl;
		return (-1);
	}
		
	// ANCHOR gnl로 벡터에 넣어줌
	std::vector<std::string> gnl;
	char *line;
	while (get_next_line(fd_conf, &line) > 0)
	{
		gnl.push_back(line);
		free(line);
	}
	gnl.push_back(line);
	free(line);

	// STUB 벡터에 넣어진 값 출력	
	// for (size_t i = 0; i < gnl.size(); i++)
	// {
	// 	cout << gnl[i] << endl;
	// }
	// cout << gnl.size() << endl;
	cout << " ------------ ==== ------------" << endl;

	// if (-1 == server.SetServer(gnl))
	if (-1 == SetServer(server, gnl))
	{
		cout << ("no server block") << endl;
		return (-1);
	}
	
	// NOTE 값이 제대로 들어왔나 출력해보기
	server.ShowServerConfigs();
	close(fd_conf);
	return (1);
}
