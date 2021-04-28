#pragma once
#include <string>
#include <unistd.h>
#include <iostream>

size_t				strlen(const char* str);
int					strncmp(char* a, char* b, size_t len);
bool	get_seq_token(const std::string &origin, std::string::const_iterator &it, std::string &token, char *seq, size_t	reserve_size = 1000);
int		get_set_token(const std::string &origin, std::string::const_iterator &it, std::string &token, char *set, size_t	reserve_size = 1000);
bool	get_chr_token(const std::string &origin, std::string::const_iterator &it, std::string &token, char c, size_t reserve_size = 1000);
void*				ft_memset(void *b, int c, size_t len);