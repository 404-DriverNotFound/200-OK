#pragma once
#include <vector>
#include <string>

class Path
{
	private:
		std::vector<std::string>	segments;

		const std::string			getPath2(std::vector<std::string> const &vec) const;
		void						setPath2(std::vector<std::string> &vec, const std::string& str);
	
	public:
		Path();
		Path(std::string const &);
		Path(Path const &);
		~Path();
		const std::string			getPath() const;
		void						setPath(const std::string&);
		const int					getSize() const;
		
		Path&						operator+(Path&);
		Path&						operator--();     // prefix "--"
		Path&						operator--(int);  // prefix "--"
		Path&						operator=(const std::string&);
		Path&						operator=(const Path&);
};

