#pragma once
#include "../../gnl/get_next_line_bonus.hpp"
#include <vector>
#include <string>
#include <iostream>

class Path
{
public:
	Path();
	Path(std::string const&);
	Path(Path const&);
	~Path();
	const std::string			getPath() const;
	void						setPath(const std::string&);
	int							getSize() const;
	
	Path&						operator+(Path&);
	Path&						operator--();     // 전위 prefix "--Path"
	Path						operator--(int);  // 후위 postfix "Path--"
	Path&						operator=(const std::string&);
	Path&						operator=(const Path&);
	bool						operator==(const Path&);
	std::string&				operator[](std::size_t i);

	class OutOfRange : public std::exception
	{
		public:
			const char* what() const throw();
	};
	
private:
	std::vector<std::string>	segments;

	const std::string			getPath2(std::vector<std::string> const &vec) const;				// NOTE getPath의 sub 함수
	void						setPath2(std::vector<std::string> &vec, const std::string& str);	// NOTE setPath의 sub 함수
};
