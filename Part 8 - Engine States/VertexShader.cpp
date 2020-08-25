#include "VertexShader.h"
#include <iostream>

VertexShader::VertexShader()
{
}

VertexShader::~VertexShader()
{
}

ID3D11VertexShader* VertexShader::getShader()
{
	return this->vertexShader;
}

void VertexShader::release()
{
	this->vertexShader->Release();
	delete this;
}

//requires the shader file
void VertexShader::init(void* shaderByteCode, size_t byteCodeSize)
{
	ID3D11Device* directXDevice = GraphicsEngine::getInstance()->getDirectXDevice();
	HRESULT vertexResult =  directXDevice->CreateVertexShader(shaderByteCode, byteCodeSize, NULL, &this->vertexShader);
	if (SUCCEEDED(vertexResult)) {
		std::cout << "Created runtime vertex shader successfully. \n";
	}
	else {
		std::cout << "An error occured during creating of runtime vertex shader. \n";
	}

}
