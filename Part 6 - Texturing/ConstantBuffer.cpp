#include "ConstantBuffer.h"
#include "GraphicsEngine.h"

ConstantBuffer::ConstantBuffer()
{
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::load(void* bufferPtr, UINT byteShaderSize)
{
	//release if previous buffer and input layout are used from previous frame update.
	if (this->buffer != NULL) {
		this->buffer->Release();
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = byteShaderSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = bufferPtr;

	ID3D11Device* directXDevice = GraphicsEngine::getInstance()->getDirectXDevice();
	HRESULT bufferResult = directXDevice->CreateBuffer(&bufferDesc, &initData, &this->buffer);
	if (SUCCEEDED(bufferResult)) {
		std::cout << "Creation of constant buffer succeeded. \n";
	}
	else {
		std::cout << "An error occurred in creating a constant buffer. \n";
	}

}

void ConstantBuffer::update(DeviceContext* context, void* buffer)
{
	context->getContext()->UpdateSubresource(this->buffer, NULL, NULL, buffer, NULL, NULL);
}

void ConstantBuffer::release()
{
}

ID3D11Buffer* ConstantBuffer::getBuffer()
{
	return this->buffer;
}
