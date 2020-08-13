#pragma once
#include "Resource.h"

class Texture : public AResource
{
public:
	Texture(const wchar_t* fullPath);
	~Texture();
	AResource::String getPath();
};

