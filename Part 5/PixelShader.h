#pragma once
#include "GraphicsEngine.h"

class PixelShader
{
public: 
	PixelShader();
	~PixelShader();

	ID3D11PixelShader* getShader();

	void release();
private:
	void init(void* shaderByteCode, size_t byteCodeSize);
	ID3D11PixelShader* pixelShader = NULL;

	friend class GraphicsEngine;
};

