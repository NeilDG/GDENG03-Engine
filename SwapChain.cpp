#include "SwapChain.h"

SwapChain::SwapChain(ID3D11Device* directXDevice)
{
	this->directXDevice = directXDevice;
}

SwapChain::~SwapChain()
{
}

bool SwapChain::init(HWND windowHandle, UINT width, UINT height)
{
	GraphicsEngine::getInstance()->initializeSwapChain(windowHandle, this->chainRef, width, height);

	ID3D11Texture2D* buffer;
	ZeroMemory(&buffer, sizeof(buffer));

	this->chainRef->GetBuffer(0, __uuidof(ID3D11Texture3D), (void**)buffer);
	HRESULT hr = this->directXDevice->CreateRenderTargetView(buffer, NULL, &this->renderView);
	buffer->Release();

	return SUCCEEDED(hr);
}

void SwapChain::present(bool vsync)
{
	this->chainRef->Present(vsync, NULL);
}

bool SwapChain::release()
{
	if (this->chainRef != NULL) {
		this->chainRef->Release();
		delete this;
		return true;
	}
}

ID3D11RenderTargetView* SwapChain::getRenderTargetView()
{
	return this->renderView;
}
