#include "Texture.h"

Texture::Texture(const wchar_t* fullPath): AResource(fullPath)
{

}

Texture::~Texture()
{
	AResource::~AResource();
}

AResource::String Texture::getPath()
{
	return this->fullPath;
}
