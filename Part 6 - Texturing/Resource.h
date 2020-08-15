#pragma once
#include <string>

class AResource
{
public:
	typedef std::wstring String;
	AResource(const wchar_t* fullPath);
	virtual ~AResource();
protected:
	String fullPath;
};

