#include "../all_header.hpp"
#include "./server.hpp"
#include "../gnl/get_next_line_bonus.hpp"

using namespace std;

int error_return(char const * str)
{
	cout << str << endl;
	exit(EXIT_FAILURE);
	return (-1);
}

void ft_split_vector(std::vector<std::string> &vec, const std::string& str)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find(" ", before)) != std::string::npos)  //from을 찾을 수 없을 때까지
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

void outdentTab(std::string &str)
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


#define START 0
#define END 1

class locationBracket
{
	public:
		int start;
		int end;
		Config config;
		locationBracket();
		virtual ~locationBracket();
};

locationBracket::locationBracket() : start(0), end(0) {};
locationBracket::~locationBracket() {};

class serverBracket
{
	public:
		int start; // start = 0
		int end; // end = 0 
		// start = 0 & end = 0 이면, server Bracket 설정이 제대로 안된 상황
		std::vector<locationBracket> locationBracket;
		int locationNum;
		
		serverBracket();
		virtual ~serverBracket();
};

serverBracket::serverBracket() : locationNum(0), start(0), end(0){}
serverBracket::~serverBracket(){}


class ServerConfig
{
	public :
		std::vector<serverBracket> mserverBracket;
		int serverNum;
		ServerConfig();
		virtual ~ServerConfig();
		
		bool check_bracket_syntax(std::vector<std::string> &gnl, int start, int end);
		void SetServerBracket(std::vector<std::string> &gnl, ServerConfig &configs, int start);
		void SetLocationBracket(std::vector<std::string> &gnl, ServerConfig &configs, int server_idx, int start, int end);
};

bool ServerConfig::check_bracket_syntax(std::vector<std::string> &gnl, int start, int end)
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

void ServerConfig::SetServerBracket(std::vector<std::string> &gnl, ServerConfig &configs, int start)
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
			configs.serverNum++;
			configs.mserverBracket.push_back(temp);
			return;
		}
	}
}

void ServerConfig::SetLocationBracket(std::vector<std::string> &gnl, ServerConfig &configs, int server_idx, int start, int end)
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
			configs.mserverBracket[server_idx].locationNum++;
			configs.mserverBracket[server_idx].locationBracket.push_back(temp);
			return ;
		}
		if (gnl[start].size() != 1)
			outdentTab(gnl[start]);
		start++;
	}
}


ServerConfig::ServerConfig() : serverNum(0){}
ServerConfig::~ServerConfig(){}

int Step2_1(std::vector<std::string> &gnl, ServerConfig &configs, int start)
{
	// error 처리는 잘 안함 ({}가 여러번 들어온다던지)
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
			configs.serverNum++;
			configs.mserverBracket.push_back(temp);
			return (true);
		}
	}
}

// serverConfig를 채워넣음 파싱 첫 단계
int Step2(ServerConfig &configs, std::vector<std::string> &gnl)
{
	int i = 0;
	int j = 0;
	while (i < gnl.size())
	{
		if (gnl[i].compare("server") == 0) // server block 단위 하나 처리
		{
			if (configs.check_bracket_syntax(gnl, i + 1, gnl.size()) == true)
			{
				// if (Step2_1(gnl, configs, i) == false)
				// 	return (-1);
				// else
				// 	i = configs.serverBracket[configs.serverNum - 1].end;
				configs.SetServerBracket(gnl, configs, i + 1);
				
				i = configs.mserverBracket[configs.serverNum - 1].end;
			}
		}
		i++;
	}
	// cout << configs.serverNum << endl;
	// cout << configs.serverBracket[0].locationNum << endl;
	// cout << configs.serverBracket[0].start << endl;
	// cout << configs.serverBracket[0].end << endl;

}


int Step3_1(std::vector<std::string> &gnl, ServerConfig &configs, int server_idx, int start, int end)
{
	// error 처리는 잘 안함
	bool startCheck = false;
	bool endCheck = false;
	locationBracket temp; temp.start = 0; temp.end = 0;
	// configs.serverBracket[server_idx].locationNum = 1;
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
			configs.mserverBracket[server_idx].locationNum++;
			configs.mserverBracket[server_idx].locationBracket.push_back(temp);
			return (true);
		}
		if (gnl[start].size() != 1)
			outdentTab(gnl[start]);
		start++;
	}
	if (!(startCheck == true && endCheck == true))
		return (false);
}


int Step3(ServerConfig &configs, std::vector<std::string> &gnl)
{
	for (int serverNum = 0; serverNum < configs.serverNum; serverNum++)
	{
		int start = configs.mserverBracket[serverNum].start + 1;
		int end = configs.mserverBracket[serverNum].end - 1;
		int temp = start;
		configs.mserverBracket[serverNum].locationNum = 0;
		while (temp <= end)
			outdentTab(gnl[temp++]);
		while (start <= end)
		{
			cout << "debug:" << gnl[start] << endl;
			if (gnl[start].find("location ") != std::string::npos) // server block 단위 하나 처리
			{
				cout << "ㅗㅗㅗㅗㅗㅗㅗㅗㅗㅗㅗㅗ" << endl;
				if (configs.check_bracket_syntax(gnl, start + 1, end) == true)
				{
					// cout << "ㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜㅜ" << endl;
					// if (Step3_1(gnl, configs, serverNum, start, end) == false) // NOTE {}가 짝을 이루지 않는 경우여러번 들어온다던지)
					// {
					// 	cout << "righ?" << endl;
					// 	// cout << start << endl;
					// 	return (-1);
					// }
					// else
					// {
					// 	int location_idx = configs.mserverBracket[serverNum].locationNum - 1;
					// 	cout << "location_idx: " << location_idx << endl;
					// 	cout << "asdfljsdalfkadjslfadsjlfadsj"	 << endl;
					// 	if (location_idx < 0)
					// 	{
					// 		cout << " here is it " << endl;
					// 		return (1);
					// 	}
					// 	start = configs.mserverBracket[serverNum].locationBracket[location_idx].end;
					// 	cout << "start: " << start << endl;
					// }
					
					configs.SetLocationBracket(gnl, configs, serverNum, start, end);
					int location_idx = configs.mserverBracket[serverNum].locationNum - 1;
					cout << "location_idx: " << location_idx << endl;
					start = configs.mserverBracket[serverNum].locationBracket[location_idx].end ; // NOTE location '}' 이후에 바로 다음줄에 location이 나오는 경우 때문에 '+1'을 해주지 않음
					cout << "start: " << start << endl;
				}
			}
			start++;
		}
	}
}


int Step1(Server &servers, std::vector<std::string> &gnl)
{
	std::string oneline;
	std::vector<std::string> split_vector;
	ServerConfig idx_configs;
	//서버 단위 단위로 나눠야한다.
	if (-1 == Step2(idx_configs, gnl))
	{
		cout << "a" << endl;
		return (-1);
	}
	if (-1 == Step3(idx_configs, gnl)) // Targeting
	{
		cout << "b" << endl;		
		return (-1);
	}
		
	//STUB 출력: 서버 범위
	for (size_t i = 0; i < idx_configs.serverNum; i++)
	{
		int start = 0; int end = 0;
		Config default_config;
		start = idx_configs.mserverBracket[i].start + 1;
		end = idx_configs.mserverBracket[i].end - 1;
		while (start <= end)
		{
			// 매칭 하나하나 시키면서 default location을 생성함
			cout << gnl[start] << endl;			
			// ft_split_vector(split_vector, gnl[start]);
			start++;
		}
		//생성 후 할당 register
		
		/* location 갯수만큼 loop */
		// for (size_t j = 0; j < idx_configs.mserverBracket[i].locationNum; j++)
		// {
		// 	Config sub_config;
		// 	// sub_config = default_config; 복사생성자 혹은 operator=
		// }
	}
	
	//STUB 출력하는 부분
	cout << "Total serverNum: "<< idx_configs.serverNum << endl;
	for (size_t i = 0; i < idx_configs.serverNum; i++)
	{
		cout << "location num: " << idx_configs.mserverBracket[i].locationNum << endl;
		for (size_t j = 0; j < idx_configs.mserverBracket[i].locationNum; j++)
		{
			cout << "location start: " << idx_configs.mserverBracket[i].locationBracket[j].start << endl;
			cout << "location end  : " << idx_configs.mserverBracket[i].locationBracket[j].end << endl;			
		}
		cout << " =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
	}
	return (1);
}

int main()
{
	Server server;
	int server_config;
	char buffer[BUFSIZ];
	// server_config = open("server_config3", O_RDWR);
	server_config = open("/Users/yunslee/webserv_200/yunslee_test/server_parsing/server_config3", O_RDWR);
	
	if (server_config < 0)
	{
		cout << "---" << server_config << endl;
		perror("??: ");
		return (error_return("fd open error"));
	}
	cout << "---" << server_config << endl;
	int read_size;
	std::vector<std::string> gnl;
	char *line;
	while (get_next_line(server_config, &line) > 0)
	{
		gnl.push_back(line);
		free(line);
	}
	gnl.push_back(line);
	free(line);
	
	// for (size_t i = 0; i < gnl.size(); i++)
	// {
	// 	cout << gnl[i] << endl;
	// }
	// cout << gnl.size() << endl;
	cout << " ------------ read function ------------" << endl;

	if (-1 == Step1(server, gnl))
		return (error_return("no server block"));

	close(server_config);
	return (1);
}
