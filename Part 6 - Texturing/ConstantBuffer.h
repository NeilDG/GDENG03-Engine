#pragma once
#include <d3d11.h>
#include "DeviceContext.h"

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void load(void* bufferPtr, UINT byteShaderSize);
	void update(DeviceContext* context, void* buffer);
	void release();

	ID3D11Buffer* getBuffer();

private:
	ID3D11Buffer* buffer;

};

