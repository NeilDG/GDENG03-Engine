#pragma once
#include "GraphicsEngine.h"

class SwapChain
{
public: 
	void init(HWND windowHandle, UINT width, UINT height);
	void present(bool vsync);
	bool release();

	ID3D11RenderTargetView* getRenderTargetView();
	ID3D11DepthStencilView* getDepthStencilView();


private:
	ID3D11Device* directXDevice = NULL;
	IDXGISwapChain* chainRef = NULL;
	ID3D11RenderTargetView* renderView = NULL;
	ID3D11DepthStencilView* depthView = NULL;

	SwapChain(ID3D11Device* directXDevice);
	~SwapChain();

	friend class GraphicsEngine;
};

