/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yunslee <yunslee@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/07/05 18:51:55 by yunslee           #+#    #+#             */
/*   Updated: 2021/04/18 19:45:01 by yunslee          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_HPP
# define GET_NEXT_LINE_BONUS_HPP
# include <unistd.h>
# include <stdlib.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

int		get_next_line_sub(int fd, char **line);
int		get_next_line(int fd, char **line);
int		read_until_newline(char *buffer, int fd,
							char **oneline_string, char **remain_string);
int		change_newline_to_zero(char *buffer);
int exist_newline_in_remain_string(char **remain_string, int fd, char **line);

char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strdup(const char *str);
void	*ft_memset(void *dest, int c, size_t len);
char	*ft_strchr(const char *s, int c);
size_t	ft_strlen(const char *s);
#endif
