#include "./ServerConfigIdx.hpp"

using namespace std;

//ANCHOR class ServerConfigIdx 멤버 변수와 함수

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

bool ServerConfigIdx::checkBracketSyntax(std::vector<std::string> &gnl, int start, int end)
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
	std::size_t	i = start;
	bool startCheck = false;
	bool endCheck = false;
	ServerBracket temp;
	while (i < gnl.size())
	{
		if (gnl[i].size() == 1 && gnl[i].compare("{") == 0)
		{
			temp.mStart = i;
			startCheck = true;
		}
		else if (gnl[i].size() == 1 && gnl[i].compare("}") == 0)
		{
			temp.mEnd = i;
			endCheck = true;
		}
		i++;
		if (startCheck == true && endCheck == true)
		{
			configidx.mServerNum++;
			configidx.mServerBracket.push_back(temp);
			return;
		}
	}
}

void ServerConfigIdx::SetLocationBracket(std::vector<std::string> &gnl, ServerConfigIdx &configidx, int server_idx, int start, int end)
{
	bool startCheck = false;
	bool endCheck = false;
	LocationBracket temp;
	while (start <= end)
	{
		if (gnl[start].size() == 1 && gnl[start].compare("{") == 0)
		{
			temp.mStart = start;
			startCheck = true;
		}
		else if (gnl[start].size() == 1 && gnl[start].compare("}") == 0)
		{
			temp.mEnd = start;
			endCheck = true;
		}
		if (startCheck == true && endCheck == true)
		{
			configidx.mServerBracket[server_idx].mLocationBlockNum++;
			configidx.mServerBracket[server_idx].mLocationBracket.push_back(temp);
			return ;
		}
		if (gnl[start].size() != 1)
			outdentTab(gnl[start]);
		start++;
	}
}

int ServerConfigIdx::Step1(ServerConfigIdx &configidx, std::vector<std::string> &gnl)
{
	std::size_t	i = 0;
	while (i < gnl.size())
	{
		if (gnl[i].compare("server") == 0) // server block 단위 하나 처리
		{
			if (configidx.checkBracketSyntax(gnl, i + 1, gnl.size()) == true)
			{
				configidx.SetServerBracket(gnl, configidx, i + 1);
				i = configidx.mServerBracket[configidx.mServerNum - 1].mEnd;
			}
			else
				return (-1);
		}
		i++;
	}
	return (1);
}

int ServerConfigIdx::Step2(ServerConfigIdx &configidx, std::vector<std::string> &gnl)
{
	for (int serverNum = 0; serverNum < configidx.mServerNum; serverNum++)
	{
		int start = configidx.mServerBracket[serverNum].mStart + 1;
		int end = configidx.mServerBracket[serverNum].mEnd - 1;
		int temp = start;
		configidx.mServerBracket[serverNum].mLocationBlockNum = 0;
		while (temp <= end)
			configidx.outdentTab(gnl[temp++]);
		while (start <= end)
		{
			if (gnl[start].find("location ") != std::string::npos) // server block 단위 하나 처리
			{
				std::vector<std::string> temp;
				std::string mLocationPath;
				ft::splitVector(temp, gnl[start], " ");
				if (temp.size() == 2)
				{
					mLocationPath = temp[1];
					if (configidx.checkBracketSyntax(gnl, start + 1, end) == true) // NOTE { 로 시작하는 라인을 index(=start+1)로 넘겨줌
					{
						
						configidx.SetLocationBracket(gnl, configidx, serverNum, start, end);
						int location_idx = configidx.mServerBracket[serverNum].mLocationBlockNum - 1;
						configidx.mServerBracket[serverNum].mLocationBracket[location_idx].mLocationPath = mLocationPath;
						// cout << "mLocationPath: " << configidx.mServerBracket[serverNum].mLocationBracket[location_idx].mLocationPath.getPath() << endl;
						start = configidx.mServerBracket[serverNum].mLocationBracket[location_idx].mEnd; // NOTE location '}' 이후에 바로 다음줄에 location이 나오는 경우 때문에 '+1'을 해주지 않음
					}
					else
						return (-1);
				}
				else
					return (-1);
			}
			start++;
		}
	}
	return (1);
}

ServerConfigIdx::ServerConfigIdx() : mServerNum(0), mTotalLocationNum(0) {}
ServerConfigIdx::~ServerConfigIdx(){}

ServerBracket::ServerBracket() : mLocationBlockNum(0), mStart(0), mEnd(0){}
ServerBracket::~ServerBracket(){}

LocationBracket::LocationBracket() : mStart(0), mEnd(0), mLocationPath() {};
LocationBracket::~LocationBracket() {};

/*
// ANCHOR 서버파싱하는데 필요한 일반함수들. 다른 파일로 분리할 수 없어서 여기에 잔존한다.
*/

int isCorrespondedServerDirective(int index, ServerBracket &ServerBracket)
{
	for (size_t i = 0; i < ServerBracket.mLocationBracket.size(); i++)
	{
		if (index >= ServerBracket.mLocationBracket[i].mStart && index <= ServerBracket.mLocationBracket[i].mEnd)
		{
			return (false);
		}
	}
	return (true);
}

int parsingserverBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end, ServerBracket &ServerBracket)
{
	std::string oneline;
	std::vector<std::string> splitVector;
	bool exist_index_pages = false;
	bool exist_cgi_extension = false;
	bool exist_method = false;
	int i = start;
	while (i <= end)
	{
		if (isCorrespondedServerDirective(i, ServerBracket) == false)
		{
			i++;
			continue;
		}
		oneline = gnl[i];
		splitVector.clear();
		ft::splitVector(splitVector, oneline, " ");
		if (splitVector.size() <= 1)
		{
			i++;
			continue;
		}
		
		// STUB if문 비교하기
		if (splitVector[0].compare("server_name") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mServerName = splitVector[1];
		}
		if (splitVector[0].compare("root") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			Path root(splitVector[1]);
			default_location.mRoot = root;
		}
		else if (splitVector[0].compare("port") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mPort = std::atoi(splitVector[1].c_str());
		}
		else if (splitVector[0].compare("host") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mHost = splitVector[1];
		}
		else if (splitVector[0].compare("client_max_body_size") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mClientMaxBodySize = std::atoi(splitVector[1].c_str());
		}
		else if (splitVector[0].compare("index_pages") == 0)
		{
			std::size_t	i = 1;
			std::vector<Path>	index_pages;
			while (i < splitVector.size())
			{
				
				Path index_page(splitVector[i]);
				if (exist_index_pages == true)
					default_location.mIndexPages.push_back(splitVector[i]);
				else
					index_pages.push_back(splitVector[i]);
				i++;
			}
			if (exist_index_pages == false)
			{
				default_location.mIndexPages = index_pages;
				exist_index_pages = true;
			}
		}
		else if (splitVector[0].compare("error_page") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			Path error_page(splitVector[1]);
			default_location.mErrorPage = error_page;
		}
		else if (splitVector[0].compare("autoindex") == 0)
		{
			bool value;
			if (splitVector.size() != 2)
				return (-1);
			if (splitVector[1].compare("off") == 0)
				value = false;
			if (splitVector[1].compare("on") == 0)
				value = true;
			default_location.mAutoIndex = value;
		}
		else if (splitVector[0].compare("timeout") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mTimeOut = std::atoi(splitVector[1].c_str());
		}
		else if (splitVector[0].compare("method") == 0)
		{
			if (splitVector.size() < 2)
				return (-1);
			std::size_t	i = 1;
			std::vector<std::string> method;
			while (i < splitVector.size())
			{
				if (exist_method == true)
					default_location.mMethod.push_back(splitVector[i]);
				else
					method.push_back(splitVector[i]);
				i++;
			}
			if (exist_method == false)
			{
				default_location.mMethod = method;
				exist_method = true;
			}
		}
		else if (splitVector[0].compare("cgi_extension") == 0)
		{
			if (splitVector.size() < 2)
				return (-1);
			std::size_t	i = 1;
			std::vector<std::string> cgi_extension;
			while (i < splitVector.size())
			{
				if (exist_cgi_extension == true)
					default_location.mCgiExtension.push_back(splitVector[i]);
				else
					cgi_extension.push_back(splitVector[i]);
				i++;
			}
			if (exist_cgi_extension == false)
			{
				default_location.mCgiExtension = cgi_extension;
				exist_cgi_extension = true;
			}
		}
		i++;
	}
	return (1);
}

int parsingLocationBlock(std::vector<std::string> &gnl, ConfigFile &default_location, int start, int end)
{
	std::string oneline;
	std::vector<std::string> splitVector;
	bool exist_index_pages = false;
	bool exist_cgi_extension = false;
	bool exist_method = false;
	int i = start;
	while (i <= end)
	{
		oneline = gnl[i];
		splitVector.clear();
		ft::splitVector(splitVector, oneline, " ");
		
		// STUB if문 비교하기
		if (splitVector[0].compare("server_name") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mServerName = splitVector[1];
		}
		else if (splitVector[0].compare("root") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			Path root(splitVector[1]);
			default_location.mRoot = root;
		}
		else if (splitVector[0].compare("port") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mPort = std::atoi(splitVector[1].c_str());
		}
		else if (splitVector[0].compare("client_max_body_size") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			default_location.mClientMaxBodySize = std::atoi(splitVector[1].c_str());
		}
		else if (splitVector[0].compare("index_pages") == 0)
		{
			std::size_t	i = 1;
			std::vector<Path> index_pages;
			while (i < splitVector.size())
			{
				
				Path index_page(splitVector[i]);
				if (exist_index_pages == true)
					default_location.mIndexPages.push_back(splitVector[i]);
				else
					index_pages.push_back(splitVector[i]);
				i++;
			}
			if (exist_index_pages == false)
			{
				default_location.mIndexPages = index_pages;
				exist_index_pages = true;
			}
		}
		else if (splitVector[0].compare("error_page") == 0)
		{
			if (splitVector.size() != 2)
				return (-1);
			Path error_page(splitVector[1]);
			default_location.mErrorPage = error_page;
		}
		else if (splitVector[0].compare("method") == 0)
		{
			if (splitVector.size() < 2)
				return (-1);
			std::size_t	i = 1;
			std::vector<std::string> method;
			while (i < splitVector.size())
			{
				if (exist_method == true)
					default_location.mMethod.push_back(splitVector[i]);
				else
					method.push_back(splitVector[i]);
				i++;
			}
			if (exist_method == false)
			{
				default_location.mMethod = method;
				exist_method = true;
			}
		}
		else if (splitVector[0].compare("autoindex") == 0)
		{
			bool value;
			if (splitVector.size() != 2)
				return (-1);
			if (splitVector[1].compare("off") == 0)
				value = false;
			if (splitVector[1].compare("on") == 0)
				value = true;
			default_location.mAutoIndex = value;
		}
		else if (splitVector[0].compare("cgi_extension") == 0)
		{
			if (splitVector.size() < 2)
				return (-1);
			std::size_t	i = 1;
			std::vector<std::string> cgi_extension;
			while (i < splitVector.size())
			{
				if (exist_cgi_extension == true)
					default_location.mCgiExtension.push_back(splitVector[i]);
				else
					cgi_extension.push_back(splitVector[i]);
				i++;
			}
			if (exist_cgi_extension == false)
			{
				default_location.mCgiExtension = cgi_extension;
				exist_cgi_extension = true;
			}
		}
		i++;
	}
	return (1);
}