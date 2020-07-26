#pragma once
#include <d3d11.h>
#include <iostream>
#include "DeviceContext.h"
#include "VertexBuffer.h"
#include <d3dcompiler.h>

class SwapChain;
class GraphicsEngine
{
public: 
	static GraphicsEngine* getInstance();
	static void initialize();
	static void destroy();

	SwapChain* createSwapChain();
	DeviceContext* getImmediateContext();
	IDXGIFactory* getDirectXFactory();
	ID3D11Device* getDirectXDevice();
	VertexBuffer* createVertexBuffer();

	void createShaders();
	void setShaders();

	void getShaderBufferAndSize(void** bytecode, UINT* size);


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

	ID3DBlob* m_vsblob = nullptr;
	ID3DBlob* m_psblob = nullptr;
	ID3D11VertexShader* m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;
};

