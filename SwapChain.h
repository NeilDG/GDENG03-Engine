#pragma once
#include "GraphicsEngine.h"

class SwapChain
{
public: 
	bool init(HWND windowHandle, UINT width, UINT height);
	bool release();

private:
	IDXGISwapChain* chainRef;
	SwapChain();
	~SwapChain();

	friend class GraphicsEngine;
};

