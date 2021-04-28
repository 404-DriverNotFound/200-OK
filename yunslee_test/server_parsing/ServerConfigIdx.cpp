#include "./ServerConfigIdx.hpp"

using namespace std;
void PrintServer(Server &server);


int error_return(char const * str)
{
	cout << str << endl;
	exit(EXIT_FAILURE);
	return (-1);
}

void ft_split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find(delim, before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		// cout << "   str: " << str << endl;
		// cout << "substr: " << substr << endl;
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// usr/yunslee/index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}

void setPath(std::vector<std::string> &vec, const std::string& str)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find("/", before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		cout << "   str: " << str << endl;
		cout << "substr: " << substr << endl;
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// usr/yunslee/index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}

const int eDirective_num = 12;
enum eDirective
{
	ROOT, SERVER_NAME, PORT, ERROR_PAGE, INDEX,
	HEAD_LENGTH, BODY_LENGTH, AUTOINDEX, TIMEOUT, AUTH,
	LOCATION, METHOD
};

std::string directives[12] = {"root ", "server_name ", "port ", "erro_page ", "index ",
									"head_length ", "body_length ", "autoindex ", "timeout ", "auth "
										, "location ", "method "};

// std::string server_directives[12] = {"\troot ", "\tserver_name ", "\tport ", "\terro_page ", "\tindex ",
// 									"\thead_length ", "\tbody_length ", "\tautoindex ", "\ttimeout ", "\tauth "
// 										, "\tlocation ", "\tmethod "};
// std::string location_directives[12] = {"\t\troot ", "\t\tserver_name ", "\t\tport ", "\t\terro_page ", "\t\tindex ",
// 									"\t\thead_length ", "\t\tbody_length ", "\t\tautoindex ", "\t\ttimeout ", "\t\tauth "
// 										, "\t\tlocation ", "\t\tmethod "};

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
	serverBracket temp;
	// int serverBracket[2]; serverBracket[START] = 0; serverBracket[END] = 0;
	while (i < gnl.size())
	{
		if (gnl[i].size() == 1 && gnl[i].compare("{") == 0)
		{
			// cout << gnl[i] << "; "<< gnl[i].size() << endl;
			temp.start = i;
			startCheck = true;
		}
		else if (gnl[i].size() == 1 && gnl[i].compare("}") == 0)
		{
			// cout << gnl[i] << "; "<< gnl[i].size() << endl;
			temp.end = i;
			endCheck = true;
		}
		i++;
		if (startCheck == true && endCheck == true)
		{
			configidx.serverNum++;
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
	locationBracket temp;
	while (start <= end)
	{
		// cout << gnl[start] << endl;
		if (gnl[start].size() == 1 && gnl[start].compare("{") == 0)
		{
			temp.start = start;
			startCheck = true;
		}
		else if (gnl[start].size() == 1 && gnl[start].compare("}") == 0)
		{
			temp.end = start;
			endCheck = true;
		}
		if (startCheck == true && endCheck == true)
		{
			configidx.mserverBracket[server_idx].locationBlockNum++;
			configidx.mserverBracket[server_idx].locationBracket.push_back(temp);
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
				i = configidx.mserverBracket[configidx.serverNum - 1].end;
			}
		}
		i++;
	}
}

int ServerConfigIdx::Step2(ServerConfigIdx &configidx, std::vector<std::string> &gnl)
{
	for (int serverNum = 0; serverNum < configidx.serverNum; serverNum++)
	{
		int start = configidx.mserverBracket[serverNum].start + 1;
		int end = configidx.mserverBracket[serverNum].end - 1;
		int temp = start;
		configidx.mserverBracket[serverNum].locationBlockNum = 0;
		while (temp <= end)
			configidx.outdentTab(gnl[temp++]);
		while (start <= end)
		{
			if (gnl[start].find("location ") != std::string::npos) // server block 단위 하나 처리
			{
				if (configidx.check_bracket_syntax(gnl, start + 1, end) == true) // NOTE { 로 시작하는 라인을 index(=start+1)로 넘겨줌
				{
					configidx.SetLocationBracket(gnl, configidx, serverNum, start, end);
					int location_idx = configidx.mserverBracket[serverNum].locationBlockNum - 1;
					start = configidx.mserverBracket[serverNum].locationBracket[location_idx].end; // NOTE location '}' 이후에 바로 다음줄에 location이 나오는 경우 때문에 '+1'을 해주지 않음
				}
			}
			start++;
		}
	}
}

int parsingServerBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end, serverBracket &serverBracket)
{
	std::string oneline;
	std::vector<std::string> split_vector;
	bool avoidLocationBlock = false;
	while (start <= end)
	{
		avoidLocationBlock = false;
		for (size_t i = 0; i < serverBracket.locationBracket.size(); i++)
		{
			if (start >= serverBracket.locationBracket[i].start && start <= serverBracket.locationBracket[i].end)
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
			default_location.server_name = split_vector[1];
		}
		else if (split_vector[0].compare("root") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path root(split_vector[1]);
			default_location.root = root;
		}
		else if (split_vector[0].compare("port") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.port = atoi(split_vector[1].c_str());
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
			default_location.index_pages = index_pages;
		}
		else if (split_vector[0].compare("error_page") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path error_page(split_vector[1]);
			default_location.error_page = error_page;
		}
		start++;
	}
	return (1);
}

int parsingBlock(std::vector<std::string> &gnl, Config &default_location, int start, int end)
{
	std::string oneline;
	std::vector<std::string> split_vector;
	
	// FIXME now
	// std::string folder = gnl[start - 1];
	// std::vector<std::string> temp;
	// ft_split_vector(temp, folder, "/");
	// folder = temp[1];
	// cout << "folder: " << endl;
	// FIXME now
	
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
			default_location.server_name = split_vector[1];
		}
		else if (split_vector[0].compare("root") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path root(split_vector[1]);
			default_location.root = root;
		}
		else if (split_vector[0].compare("port") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			default_location.port = atoi(split_vector[1].c_str());
		}
		else if (split_vector[0].compare("index_pages") == 0)
		{
			int i = 1;
			std::vector<Path> index_pages;
			while (i < split_vector.size())
			{
				// Path index_page(split_vector[i]);
				default_location.index_pages.push_back(split_vector[i]);
				i++;
			}
		}
		else if (split_vector[0].compare("error_page") == 0)
		{
			if (split_vector.size() != 2)
				return (-1);
			Path error_page(split_vector[1]);
			default_location.error_page = error_page;
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
	// for (size_t i = 0; i < configIdx.serverNum; i++)
	// {
	// 	int start = 0; int end = 0;
	// 	Config default_config;
	// 	start = configIdx.mserverBracket[i].start + 1;
	// 	end = configIdx.mserverBracket[i].end - 1;
	// 	while (start <= end)
	// 	{
	// 		cout << gnl[start] << endl;			
	// 		start++;
	// 	}
	// }
	
	// STUB 출력: location idx
	// cout << "Total serverNum: "<< configIdx.serverNum << endl;
	// for (size_t i = 0; i < configIdx.serverNum; i++)
	// {
	// 	cout << "location num: " << configIdx.mserverBracket[i].locationBlockNum << endl;
	// 	for (size_t j = 0; j < configIdx.mserverBracket[i].locationBlockNum; j++)
	// 	{
	// 		cout << "location start: " << configIdx.mserverBracket[i].locationBracket[j].start << endl;
	// 		cout << "location end  : " << configIdx.mserverBracket[i].locationBracket[j].end << endl;			
	// 	}
	// 	cout << " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
	// }
	for (size_t i = 0; i < configIdx.serverNum; i++)
	{
		configIdx.totalLocationNum += (configIdx.mserverBracket[i].locationBlockNum + 1); // NOTE location block 갯수에 default location을 더해야하기 때문에
	}
	
	
	// ANCHOR Step2. 값 넣기
	for (size_t i = 0; i < configIdx.serverNum; i++)
	{
		Config default_location; // NOTE 하나의 서버가 될 것임
		std::string oneline;
		std::vector<std::string> split_vector;
		int start = configIdx.mserverBracket[i].start + 1;
		int end = configIdx.mserverBracket[i].end - 1;
		
		if (parsingServerBlock(gnl, default_location, start, end, configIdx.mserverBracket[i]) == -1)
		{
			cout << "parsingBlock error" << endl;
			return (-1);
		}

		servers.config_locations.push_back(default_location); // NOTE server의 default location

		for (size_t j = 0; j < configIdx.mserverBracket[i].locationBlockNum ; j++)
		{
			Config temp = default_location;
			int start2 = configIdx.mserverBracket[i].locationBracket[j].start + 1;
			int end2 = configIdx.mserverBracket[i].locationBracket[j].end - 1;
			if (parsingBlock(gnl, temp, start2, end2) == -1)
			{
				cout << "parsingBlock error" << endl;
				return (-1);
			}
			servers.config_locations.push_back(temp); // NOTE 서버의 location 블록에 있는 정보를 넣는다.
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
		return (error_return("fd open error"));
		
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

	if (-1 == SetServer(server, gnl))
		return (error_return("no server block"));
		
	PrintServer(server);
	
	close(fd_conf);
	return (1);
}


void PrintServer(Server &server)
{
	for (size_t i = 0; i < server.config_locations.size(); i++)
	{
		cout << "server_name: " << server.config_locations[i].server_name << endl;
		cout << "root: " << server.config_locations[i].root.getPath() << endl;
		cout << "port: " << server.config_locations[i].port << endl;
		
		cout << "index_pages: ";
		for (size_t j = 0; j < server.config_locations[i].index_pages.size(); j++)
		{
			cout << server.config_locations[i].index_pages[j].getPath() << " ";
		}
		cout << endl;
		cout << "error_page: " << server.config_locations[i].error_page.getPath() << endl;
		cout << "-=-=-=-=-=--=-=-=-=-=--=-=-=-=-=--=-=-=-=-=-" << endl;
	}
}
