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
	
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;

	HRESULT depthResult = this->directXDevice->CreateTexture2D(&texDesc, NULL, &buffer);
	LogUtils::PrintHRResult(depthResult);

	HRESULT depthStencilResult = this->directXDevice->CreateDepthStencilView(buffer, NULL, &this->depthView);
	LogUtils::PrintHRResult(depthStencilResult);
	buffer->Release();
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

ID3D11DepthStencilView* SwapChain::getDepthStencilView()
{
	return this->depthView;
}
