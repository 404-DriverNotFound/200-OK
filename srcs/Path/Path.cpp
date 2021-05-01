#include "./Path.hpp"

void				Path::setPath2(std::vector<std::string> &vec, const std::string& str)
{
	size_t before = 0; //string처음부터 검사
	size_t after = 0;
	std::string substr;
	while((after = str.find("/", before)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		substr = str.substr(before, after - before);
		// cout << "   str: " << str << endl;
		// cout << "substr: " << substr << endl;
		if (substr.size() != 0)
			vec.push_back(substr);
		before = after + 1;
	}

	// usr/yunslee/index.html 마지막을 넣어주기 위해
	if (before < str.size())
	{
		substr = str.substr(before, after - 1);
		vec.push_back(substr);
		before = after + 1;
	}
}

const std::string	Path::getPath2(std::vector<std::string> const &vec) const
{
	std::string temp;
	for (size_t i = 0; i < vec.size(); i++)
	{
		temp.append(vec[i]);
		if (i != vec.size() - 1) // REVIEW 나중에봐서 마지막에 '/' 붙일지 말지 선택
			temp += "/";
	}
	return (temp);
}

const std::string	Path::getPath() const
{
	return (getPath2(this->segments));
}

void				Path::setPath(const std::string& str)
{
	setPath2(this->segments, str);
}

Path&				Path::operator+(Path& path)
{
	for (size_t i = 0; i < path.segments.size(); i++)
	{
		this->segments.push_back(path.segments[i]);
	}
	return (*this);
}

Path				Path::operator--(int) // 후위 postfix "Path--"
{
	Path temp = *this;
	this->segments.pop_back();
	return (temp);
}

Path&				Path::operator--() // 전위 prefix "--Path"
{
	this->segments.pop_back();
	return (*this);
}

Path::Path(){}

Path::~Path(){}

Path::Path(std::string const &str)
{
	this->setPath(str);
}

Path::Path(Path const &path)
{
	this->segments.clear();
	(*this) = path;
}

Path&				Path::operator=(const std::string& str)
{
	setPath(str);
	return (*this);
}

Path&				Path::operator=(const Path& path)
{
	if (this == &path)
		return (*this);
	this->segments = path.segments;
	return (*this);
}

const int			Path::getSize() const
{
	return (this->segments.size());
}