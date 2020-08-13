#pragma once
#include <d3d11.h>

class SwapChain;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class VertexShader;
class PixelShader;
class Texture;

class DeviceContext
{
public:
	DeviceContext(ID3D11DeviceContext* deviceContext);
	~DeviceContext();

	void clearRenderTargetColor(SwapChain* swapChain, float red, float green, float blue, float alpha);
	void setVertexBuffer(VertexBuffer* vertexBuffer);
	void setIndexBuffer(IndexBuffer* indexBuffer);

	void drawTriangle(UINT vertexCount, UINT startIndex);
	void drawTriangle(UINT indexCount, UINT startIndex, UINT startLocation);
	void drawTriangleStrip(UINT vertexCount, UINT startIndex);
	void setViewportSize(UINT width, UINT height);

	void setVertexShader(VertexShader* vertexShader);
	void setPixelShader(PixelShader* pixelShader);
	void setConstantBuffer(VertexShader* vertexShader, ConstantBuffer* buffer);
	void setConstantBuffer(PixelShader* vertexShader, ConstantBuffer* buffer);
	void setTexture(VertexShader* vertexShader, Texture* texture);
	void setTexture(PixelShader* vertexShader, Texture* texture);

	void release();
	ID3D11DeviceContext* getContext();

private:
	ID3D11DeviceContext* myContext;
};

