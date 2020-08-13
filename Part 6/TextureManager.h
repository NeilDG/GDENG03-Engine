#pragma once
#include "ResourceManager.h"
class Texture;
class TextureManager: public AResourceManager
{
private:
	Resource* convertResourceFromFile(const wchar_t* filePath) override;
};

