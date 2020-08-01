#pragma once
#include "GraphicsEngine.h"

class VertexShader
{
public:
	VertexShader();
	~VertexShader();

	ID3D11VertexShader* getShader();
	void release();
	

private:
	void init(void* shaderByteCode, size_t byteCodeSize);
	ID3D11VertexShader* vertexShader = NULL;

	friend class GraphicsEngine;
};

