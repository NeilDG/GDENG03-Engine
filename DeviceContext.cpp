#include "DeviceContext.h"
#include "SwapChain.h"

DeviceContext::DeviceContext(ID3D11DeviceContext* deviceContext): myContext(deviceContext)
{

}

DeviceContext::~DeviceContext()
{
}

void DeviceContext::clearRenderTargetColor(SwapChain* swapChain, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = {red, green, blue, alpha};
	this->myContext->ClearRenderTargetView(swapChain->getRenderTargetView(), clearColor);
}

void DeviceContext::release()
{
	this->myContext->Release();
}
