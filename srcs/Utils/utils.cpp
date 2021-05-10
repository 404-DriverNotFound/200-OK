#include "utils.hpp"

void	ft::testNameSpace(void)
{
	std::cout << "namespace working" << std::endl;
}

void	*ft::memset(void *b, int c, size_t len)
{
	while (len--)
		*((unsigned char*)b + len) = (unsigned char)c;
	return (b);
}

void ft::split_vector(std::vector<std::string> &vec, const std::string& str, const char *delim)
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

// FIXME 최종적으로 안쓰면 지우기.
// static int	atoi_while(const char *str, int i, int sign)
// {
// 	unsigned long long int	sum;

// 	sum = 0;
// 	while (str[i] >= 48 && str[i] <= 57)
// 	{
// 		sum *= 10;
// 		sum += str[i] - 48;
// 		i++;
// 	}
// 	if (sum > LLONG_MAX && sign == -1)
// 		return (0);
// 	if (sum > LLONG_MAX && sign == 1)
// 		return (-1);
// 	return ((int)sum);
// }

// int			ft::atoi(const char *str)
// {
// 	int i;
// 	int sign;

// 	sign = 1;
// 	i = 0;
// 	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
// 		i++;
// 	if (str[i] == '-')
// 	{
// 		sign *= -1;
// 		i++;
// 	}
// 	else if (str[i] == '+')
// 		i++;
// 	if (!(str[i] >= 48 && str[i] <= 57))
// 		return (0);
// 	return (sign * atoi_while(str, i, sign));
// }

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

char	*ft::itoa(int n)
{
	char	*itoa;
	int		itoa_len;
	int		i;
	int		temp;

	i = 1;
	itoa_len = ft_itoa_len(n);
	if (NULL == (itoa = (char *)malloc(sizeof(char) * (itoa_len + 1))))
		return (NULL);
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
	return (itoa);
}



/**
 * isFilePath : path가 file path인지 체크하는 함수
 * @param  {std::string} path : 경로
 * @return {bool}             : 1 : 파일 경로, 0 : 파일 경로 x
 */
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

/**
 * isDirPath : path가 directory path인지 체크하는 함수
 * @param  {std::string} path : 경로
 * @return {bool}             : 1 : 폴더 경로, 0 : 폴더 경로 x
 */
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
/**
 * getStatusStr : 상태 코드에 따른 메세지 문자열을 구해주는 함수
 * @param  {uint16_t} code : 상태 코드
 * @return {std::string}   : 상태 코드에 따른 상태 메세지 문자열
 */