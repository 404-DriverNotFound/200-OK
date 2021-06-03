#include "utils.hpp"

void	*ft::memset(void *b, int c, size_t len)
{
	while (len--)
		*((unsigned char*)b + len) = (unsigned char)c;
	return (b);
}

void ft::splitVector(std::vector<std::string> &vec, const std::string& str, const char *delim)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find(delim, before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// NOTE usr/yunslee/index.html 의 경우 마지막 index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}

std::string ft::ReplaceAll(std::string &str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0; //string처음부터 검사
    while((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
    }
    return str;
}

std::string ft::ReplaceAllModified(std::string &str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0; //string처음부터 검사
    while((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
    {
        str.replace(start_pos, from.length(), to);
        // start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
    }
    return str;
}

static int		ft_itoa_len(int v)
{
	int i;

	i = 0;
	if (v == 0)
		return (1);
	if (v < 0)
		i = 1;
	while (v != 0)
	{
		v /= 10;
		i++;
	}
	return (i);
}

std::string ft::itos(int n)
{
	char		*itoa;
	int			itoa_len;
	int			i;
	int			temp;
	std::string	temp_str; temp_str.clear();

	i = 1;
	itoa_len = ft_itoa_len(n);
	if (NULL == (itoa = (char *)malloc(sizeof(char) * (itoa_len + 1))))
		return (temp_str);
	if (n < 0)
		itoa[0] = '-';
	else if (n == 0)
		itoa[0] = '0';
	itoa[itoa_len] = '\0';
	while (n != 0)
	{
		temp = n % 10;
		if (temp < 0)
			temp *= -1;
		itoa[itoa_len - i] = temp + '0';
		n /= 10;
		i++;
	}
	temp_str = itoa;
	free(itoa);
	return (temp_str);
}

u_int32_t ft::ft_htonl(u_int32_t addr)
{
	int num = 1;
	bool little_endian = 0;
	if (*(char *)&num == 1)
	{
		little_endian = true;
		u_int32_t res = (((((unsigned long)(addr) & 0xFF)) << 24) | \
						((((unsigned long)(addr) & 0xFF00)) << 8) | \
						((((unsigned long)(addr) & 0xFF0000)) >> 8) | \
						((((unsigned long)(addr) & 0xFF000000)) >> 24));
		return (res);
	}
	else
	{
		little_endian = false;
		return ((u_int32_t)addr);
	}
}


// NOTE 정상적으로 들어왔을 때를 가정함. (.... | 2.3..4.)
in_addr_t	 ft::ft_inet_addr(const char *ip_addr)
{
	if (ip_addr == NULL)
		return (0);
	std::string ip_string; ip_string.assign(ip_addr);
	std::vector<std::string> ip_dot_divid;
	ft::splitVector(ip_dot_divid, ip_string, ".");
	if (ip_dot_divid.size() != 4)
		return (0);
	u_int32_t ip_host = 0;
	ip_host += std::atoi(ip_dot_divid[0].c_str()) << 24;
	ip_host += std::atoi(ip_dot_divid[1].c_str()) << 16;
	ip_host += std::atoi(ip_dot_divid[2].c_str()) << 8;
	ip_host += std::atoi(ip_dot_divid[3].c_str());
	u_int32_t ip_network = ft_htonl(ip_host);
	return (ip_network);
}

u_int16_t ft::ft_htons(u_int16_t addr)
{
	int num = 1;
	bool little_endian = 0;
	if (*(char *)&num == 1)
	{
		little_endian = true;
		u_int16_t res =((((u_int16_t)(addr) & 0xFF)) << 8) | ((((u_int16_t)(addr) & 0xFF00)) >> 8);
		return (res);
	}
	else
	{
		little_endian = false;
		return ((u_int16_t)addr);
	}
}

// NOTE nginx에서 autoindex로 나오는 페이지의 html을 베껴썼다.
std::string ft::makeAutoindexHTML(std::string url)
{
	DIR *dir;
	struct dirent *curr;
	std::string res;
	std::string root_string;

	char root_char[300];
	if (NULL == getcwd(root_char, 300))
		return (res);
	root_string = root_char;
	// url = this->_request.getHeaderValue("Host") + this->_request.getStartLine().path;
	// dir = opendir(root_string.c_str());
	std::string full_path = root_string + url;
	dir = opendir(full_path.c_str());
	std::cout << "rootstring: " << root_string << std::endl;
	res += "<html>\n";
	res += "<head><title>Index of "; res+= url; res+= "</title></head>";
	res += "<body>\n";
	res += "<h1>Index of "; res+= url; res+= "</h1><hr><pre>\n";
	while ((curr = readdir(dir)) != NULL)
	{
		if (curr->d_name[0] != '.')
		{
			res += "<a href=\"http://";
			res += "localhost:8000"; res +=url; res += curr->d_name; res += "\">";
			res += curr->d_name; res += "</a><br>\n";
		}
		// cout << "1" << endl;
	}
	closedir(dir);
	res += "</pre></hr></body>\n</html>\n";
	return (res);
}


std::vector<int> ft::getVectorChangedFD(struct fd_set *fdset)
{
	std::vector<int> ret;
	for (size_t i = 0; i < 1024; i++)
	{
		if (FD_ISSET(i, fdset) != 0)
		{
			ret.push_back((int)i);
			std::cout << i << " ";
		}
	}
	for (size_t i = 0; i < 32; i++)
	{
		if (fdset->fds_bits[i] != 0)
		{
			std::cout << "32번째중에서 " << i << "번째, " << "fds_bits(4byte): " << fdset->fds_bits[i] << std::endl;
		}
	}
	if (ret.size() == 0)
	{
		std::cout << "empty | size: 0" << std::endl;
	}
	else
	{
		std::cout << "| size: " << ret.size() << std::endl;
	}
	return (ret);
}


std::string ft::getHTTPTimeFormat(time_t time)
{
	char s[150];
	struct tm *tm_time = std::gmtime(&time);

	strftime(s, sizeof(s), "%a, %d %b %Y %T GMT", tm_time);
    return (s);
}

std::string ft::getCurrentTime()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return (getHTTPTimeFormat(time.tv_sec));
}


unsigned long	ft::stohex(const std::string &str)
{
	unsigned long	result = 0;
	int				index = 0;

	while ((('0' <= str[index] && str[index] <= '9') ||
			('A' <= str[index] && str[index] <= 'F') ||
			('a' <= str[index] && str[index] <= 'f')) && static_cast<int>(str.length()) > index )
	{
		result *= 16;
		if (str[index] <= '9')
			result += (str[index] - '0');
		else if (str[index] <= 'F')
			result += (str[index] - 'A' + 10);
		else
			result += (str[index] - 'a' + 10);
		index++;
	}
	return result;
}


std::string ft::GetBodyFromFile(std::string uriPlusFile)
{
	std::string body;
	int fd = open(uriPlusFile.c_str(), O_RDONLY);
	if (fd == -1)
		return (body);
	off_t sz_file;
	sz_file  = lseek(fd, 0, SEEK_END);
	// printf("file size = %d\n", (int)sz_file);
	lseek(fd, 0, SEEK_SET);
	char *buffer = (char *)malloc(sizeof(char) * sz_file);
	int ret = read(fd, buffer, sz_file);
	body.append(buffer, ret);
	free(buffer);
	if (fd != -1)
		close(fd);
	return (body);
}

std::string ft::GetBodyFromFd(int fd)
{
	std::string body;
	if (fd == -1)
		return (body);
	off_t sz_file;
	sz_file  = lseek(fd, 0, SEEK_END);
	// printf( "file size = %d\n", (int)sz_file);
	lseek(fd, 0, SEEK_SET);
	char *buffer = (char *)malloc(sizeof(char) * sz_file);
	int ret = read(fd, buffer, sz_file);
	body.append(buffer, ret);
	free(buffer);
	return (body);
}

std::string		ft::inet_ntos(struct in_addr in)
{
	const in_addr_t	mask = 255;
	std::string		ret;

	ret += itos((in.s_addr & mask));
	ret += ".";
    in.s_addr >>= 8;
	ret += itos((in.s_addr & mask));
	ret += ".";
    in.s_addr >>= 8;
	ret += itos((in.s_addr & mask));
	ret += ".";
    in.s_addr >>= 8;
	ret += itos((in.s_addr & mask));
	return (ret);
}

bool ft::isFilePath(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) == 0)
	{
		if (S_ISREG(info.st_mode))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

bool ft::isDirPath(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) == 0)
	{
		if (S_ISDIR(info.st_mode))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int ft::access(std::string absolute_path, int temp)
{
	if (ft::isDirPath(absolute_path) == true)
	{
		DIR *dir;
		dir = opendir(absolute_path.c_str());
		closedir(dir); // NOTE 추가함
		if (dir == NULL)
			return (-1);
		else
			return (0);
	}
	else if (ft::isFilePath(absolute_path) == true)
	{
		int fd = open(absolute_path.c_str(), 0);
		close(fd);
		if (fd == -1)
			return (-1);
		else
			return (0);
	}
	return (-1);
	(void)temp;
}
