#pragma once
#include <d3d11.h>

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void load(void* vertexList, UINT vertexSize, UINT listSize, void* shaderByteCode, UINT byteShaderSize);
	void release();

	ID3D11Buffer* getBuffer();
	ID3D11InputLayout* getInputLayout();
	UINT getVertexSize();
	UINT getListSize();

private:
	UINT vertexSize;
	UINT listSize;
	ID3D11Buffer* buffer;
	ID3D11InputLayout* inputLayout;

};

