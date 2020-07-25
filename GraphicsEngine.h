#pragma once
#include <d3d11.h>
#include <iostream>
#include "DeviceContext.h"

class SwapChain;

class GraphicsEngine
{
public: 
	static GraphicsEngine* getInstance();
	static void initialize();
	static void destroy();

	SwapChain* createSwapChain();
	DeviceContext* GetImmediateContext();

private: 
	GraphicsEngine();
	~GraphicsEngine();
	GraphicsEngine(GraphicsEngine const&) {};             // copy constructor is private
	GraphicsEngine& operator=(GraphicsEngine const&) {};  // assignment operator is private*/
	static GraphicsEngine* sharedInstance;

	void init();
	void release();

	ID3D11Device* directXDevice;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11DeviceContext* deviceContext;
	IDXGIDevice* dxgiDevice;
	IDXGIAdapter* dxAdapter;
	IDXGIFactory* dxFactory;
	DeviceContext* customContext;

	void initializeSwapChain(HWND windowHandle, IDXGISwapChain* swapChain, UINT width, UINT height);

	friend class SwapChain;
};

