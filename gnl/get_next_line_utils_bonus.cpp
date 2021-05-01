/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yunslee <yunslee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/05 19:04:52 by yunslee           #+#    #+#             */
/*   Updated: 2021/04/18 19:46:28 by yunslee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.hpp"

char		*ft_strchr(const char *s, int c)
{
	int		i;
	char	*str;

	str = (char *)s;
	i = 0;
	while (str[i])
	{
		if (str[i] == (unsigned char)c)
			return (str + i);
		i++;
	}
	if (c == 0)
		return (str + i);
	return (0);
}

void		*ft_memset_gnl(void *dest, int c, size_t len)
{
	unsigned int	i;
	char			*str;

	i = 0;
	str = (char *)dest;
	while (i < len)
	{
		str[i] = (unsigned char)c;
		i++;
	}
	return (dest);
}

char		*ft_strdup(const char *str)
{
	char			*str_dup;
	unsigned int	i;
	unsigned int	str_len;

	if (str == NULL)//EOF까지 도달한 fd에 대해서 한번더 gnl을 실행할 때를 위해서 추가한 예외처리
	{
		str_dup = (char *)malloc(sizeof(char));
		str_dup[0] = '\0';
		return (str_dup);
	}
	str_len = ft_strlen(str);
	str_dup = (char *)malloc(sizeof(char) * str_len + 1);
	if (str_dup == 0)
		return (NULL);
	i = 0;
	while (i < str_len)
	{
		str_dup[i] = str[i];
		i++;
	}
	str_dup[str_len] = '\0';
	return (str_dup);
}

char		*ft_strjoin(char const *s1, char const *s2)
{
	unsigned int	i;
	unsigned int	j;
	char			*str_join;

	if (s1 == NULL)
		return (ft_strdup(s2));//static char*를 처음에 NULL로 초기화했기 때문에 필요함.
	else if (s2 == NULL)//지역변수 buffer의 주소이기때문에 NULL일 경우는 사실상 없음
		return (NULL);
	if (NULL == (str_join = (char *)malloc(sizeof(char) *
						(ft_strlen(s1) + ft_strlen(s2) + 1))))
		return (NULL);
	i = 0;
	while (s1[i])
	{
		str_join[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		str_join[i + j] = s2[j];
		j++;
	}
	str_join[i + j] = '\0';
	return (str_join);
}

size_t		ft_strlen(const char *s)
{
	unsigned int i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}
