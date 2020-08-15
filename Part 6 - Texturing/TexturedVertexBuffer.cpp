#include "TexturedVertexBuffer.h"
#include "GraphicsEngine.h"
#include <iostream>

TexturedVertexBuffer::TexturedVertexBuffer(): VertexBuffer()
{
}

TexturedVertexBuffer::~TexturedVertexBuffer()
{
	VertexBuffer::~VertexBuffer();
}

void TexturedVertexBuffer::load(void* vertexList, UINT vertexSize, UINT listSize, void* shaderByteCode, UINT byteShaderSize)
{
	//release if previous bufferand input layout are used from previous frame update.
		if (this->buffer != NULL) {
			this->buffer->Release();
		}

	if (this->inputLayout != NULL) {
		this->inputLayout->Release();
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = vertexSize * listSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertexList;

	this->vertexSize = vertexSize;
	this->listSize = listSize;

	ID3D11Device* directXDevice = GraphicsEngine::getInstance()->getDirectXDevice();
	HRESULT bufferResult = directXDevice->CreateBuffer(&bufferDesc, &initData, &this->buffer);
	if (SUCCEEDED(bufferResult)) {
		std::cout << "Creation of textured buffer succeeded. \n";
	}
	else {
		std::cout << "An error occurred in creating a textured buffer. \n";
	}

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		//semantic name - index, format, input slot, aligned byte offset, input slot class, instance data step rate
		//2 bytes each entry
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT layoutSize = ARRAYSIZE(layout);
	HRESULT layoutResult = directXDevice->CreateInputLayout(layout, layoutSize, shaderByteCode, byteShaderSize, &this->inputLayout);
	if (SUCCEEDED(layoutResult)) {
		std::cout << "Creation of input layout succeeded. \n";
	}
	else {
		std::cout << "An error occurred in creating an input layout. \n";
	}
}
