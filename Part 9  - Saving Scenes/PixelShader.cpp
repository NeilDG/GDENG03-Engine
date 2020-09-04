#include "PixelShader.h"
#include <iostream>
#include <sstream>
#include "Debug.h"

PixelShader::PixelShader()
{
}

PixelShader::~PixelShader()
{
}

ID3D11PixelShader* PixelShader::getShader()
{
	return this->pixelShader;
}

void PixelShader::release()
{
	this->pixelShader->Release();
	delete this;
}

//requires the shader file
void PixelShader::init(void* shaderByteCode, size_t byteCodeSize)
{
	ID3D11Device* directXDevice = GraphicsEngine::getInstance()->getDirectXDevice();
	HRESULT result = directXDevice->CreatePixelShader(shaderByteCode, byteCodeSize, NULL, &this->pixelShader);

	if (SUCCEEDED(result)) {
		std::stringstream buffer;
		buffer << "Created runtime pixel shader successfully. \n";
		Debug::Log(buffer.str());
	}
	else {
		std::cout << "An error occured during creating of runtime pixel shader. \n";
	}

}

