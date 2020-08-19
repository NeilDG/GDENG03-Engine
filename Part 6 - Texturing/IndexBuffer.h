#pragma once
#pragma once
#include <d3d11.h>

class IndexBuffer
{
public:
	IndexBuffer();
	~IndexBuffer();

	void load(void* vertexList, UINT listSize);
	void release();

	ID3D11Buffer* getBuffer();
	UINT getIndexSize();

private:
	UINT listSize;
	ID3D11Buffer* buffer;
};



