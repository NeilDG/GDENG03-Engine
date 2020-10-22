#pragma once
#include <string>
#include <sstream>
#include <vector>

class StringUtils
{
public:
	static std::vector<std::string> split(const std::string& s, char delim);
};

