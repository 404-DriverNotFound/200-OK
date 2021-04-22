#pragma once
#include <vector>
#include <string>

class Path
{
	private:
	std::vector<std::string>	segments;
	
	public:
	const string				getPath() const;
	const string				getPath2(std::vector<std::string> const &vec) const;
	void						setPath2(std::vector<std::string> &vec, const std::string& str);
	void						setPath(const std::string&);
	
	Path&						operator+(Path&);
	Path&						operator--();
	Path&						operator=(const std::string&);
};

