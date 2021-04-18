#include <iostream>
#include <vector>

using namespace std;

class Path
{
	private:
	std::vector<std::string>	segments;
	
	public:
	const char*					getPath() const;
	void						setPath(const std::string&);
	Path&						operator+(Path&);
	Path&						operator--();
	Path&						operator=(const std::string&);
};

const char*	Path::getPath() const
{
	
}

void		Path::setPath(const std::string& str)
{
	segments.push_back(str);
}

Path&		Path::operator+(Path&)
{

}

Path&		Path::operator--()
{

}

Path&		Path::operator=(const std::string&)
{

}

int main()
{
	Path temp;
	return (1);
}
