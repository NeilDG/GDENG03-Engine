#pragma once
#include "Resource.h"
#include <d3d11.h>

class Texture : public AResource
{
public:
	Texture(const wchar_t* fullPath);
	~Texture();
	AResource::String getPath();
	ID3D11ShaderResourceView* getShaderResource();

private:
	ID3D11Resource* myTexture = NULL;
	ID3D11ShaderResourceView* shaderResView = NULL;
};

