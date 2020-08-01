#include "SwapChain.h"
#include "LogUtils.h"

SwapChain::SwapChain(ID3D11Device* directXDevice)
{
	this->directXDevice = directXDevice;
}

SwapChain::~SwapChain()
{
}

void SwapChain::init(HWND windowHandle, UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc)); //fills occupied memory with zeroes
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = windowHandle;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;

	IDXGIFactory* dxFactory = GraphicsEngine::getInstance()->getDirectXFactory();
	HRESULT hr = dxFactory->CreateSwapChain(this->directXDevice, &desc, &this->chainRef);

	ID3D11Texture2D* buffer = NULL;

	HRESULT bufferResult = this->chainRef->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
	LogUtils::PrintHRResult(bufferResult);

	HRESULT renderResult = this->directXDevice->CreateRenderTargetView(buffer, NULL, &this->renderView);
	buffer->Release();

	LogUtils::PrintHRResult(renderResult);
	
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
