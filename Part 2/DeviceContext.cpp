#include "DeviceContext.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"

DeviceContext::DeviceContext(ID3D11DeviceContext* deviceContext): myContext(deviceContext)
{

}

DeviceContext::~DeviceContext()
{
}

void DeviceContext::clearRenderTargetColor(SwapChain* swapChain, float red, float green, float blue, float alpha)
{
	FLOAT clearColor[] = {red, green, blue, alpha};
	ID3D11RenderTargetView* renderView = swapChain->getRenderTargetView();
	this->myContext->ClearRenderTargetView(renderView, clearColor);
	this->myContext->OMSetRenderTargets(1, &renderView, NULL);
}

void DeviceContext::setVertexBuffer(VertexBuffer* vertexBuffer)
{
	UINT stride = vertexBuffer->getVertexSize();
	UINT offset = 0;
	ID3D11Buffer* buffer = vertexBuffer->getBuffer();
	this->myContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	this->myContext->IASetInputLayout(vertexBuffer->getInputLayout());
}

void DeviceContext::drawTriangle(UINT vertexCount, UINT startIndex)
{
	this->myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->myContext->Draw(vertexCount, startIndex);
}

void DeviceContext::drawTriangleStrip(UINT vertexCount, UINT startIndex)
{
	this->myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->myContext->Draw(vertexCount, startIndex);
}

void DeviceContext::setViewportSize(UINT width, UINT height)
{
	D3D11_VIEWPORT vp = {}; //IMPORTANT: all structs in Direct3D must have this to initialize any default values.
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	this->myContext->RSSetViewports(1, &vp);
}

void DeviceContext::setVertexShader(VertexShader* vertexShader)
{
	this->myContext->VSSetShader(vertexShader->getShader(), NULL, 0);
}

void DeviceContext::setPixelShader(PixelShader* pixelShader)
{
	this->myContext->PSSetShader(pixelShader->getShader(), NULL, 0);
}

void DeviceContext::setConstantBuffer(VertexShader* vertexShader, ConstantBuffer* buffer)
{
	ID3D11Buffer* deviceBuffer = buffer->getBuffer();
	this->myContext->VSSetConstantBuffers(0, 1, &deviceBuffer);
}

void DeviceContext::setConstantBuffer(PixelShader* vertexShader, ConstantBuffer* buffer)
{
	ID3D11Buffer* deviceBuffer = buffer->getBuffer();
	this->myContext->PSSetConstantBuffers(0, 1, &deviceBuffer);
}

void DeviceContext::release()
{
	this->myContext->Release();
}

ID3D11DeviceContext* DeviceContext::getContext()
{
	return this->myContext;
}
