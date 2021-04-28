#pragma once
#include <vector>
#include <string>

class Path
{
	private:
		std::vector<std::string>	segments;

		const std::string			getPath2(std::vector<std::string> const &vec) const; // NOTE getPath의 sub 함수
		void						setPath2(std::vector<std::string> &vec, const std::string& str); // NOTE setPath의 sub 함수
	
	public:
		Path();
		Path(std::string const &);
		Path(Path const &);
		~Path();
		const std::string			getPath() const;
		void						setPath(const std::string&);
		const int					getSize() const;
		
		Path&						operator+(Path&);
		Path&						operator--();     // 전위 prefix "--Path"
		Path&						operator--(int);  // 후위 postfix "Path--"
		Path&						operator=(const std::string&);
		Path&						operator=(const Path&);
};

