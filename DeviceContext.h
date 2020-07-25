#pragma once
#include <d3d11.h>

class SwapChain;

class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* deviceContext);
	~DeviceContext();

	void clearRenderTargetColor(SwapChain* swapChain, float red, float green, float blue, float alpha);
	void release();
private:
	ID3D11DeviceContext* myContext;
};

