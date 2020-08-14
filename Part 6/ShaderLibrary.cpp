#include "ShaderLibrary.h"
#include "GraphicsEngine.h"
#include "VertexShader.h"
#include "PixelShader.h"

ShaderLibrary* ShaderLibrary::sharedInstance = NULL;

ShaderLibrary* ShaderLibrary::getInstance()
{
    return sharedInstance;
}

void ShaderLibrary::initialize()
{
    sharedInstance = new ShaderLibrary();
}

void ShaderLibrary::destroy()
{
    delete sharedInstance;
}

void ShaderLibrary::requestVertexShaderData(String vertexShaderName, void** shaderByteCode, size_t* sizeShader)
{
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();
	graphEngine->compileVertexShader(vertexShaderName.c_str(), "main", shaderByteCode, sizeShader);
}

VertexShader* ShaderLibrary::getVertexShader(String vertexShaderName)
{
	if (this->activeVertexShaders[vertexShaderName] == NULL) {
		std::cout << "Vertex shader " << vertexShaderName.c_str() << " not found. Have you initialized it? \n";
	}
	return this->activeVertexShaders[vertexShaderName];
}

PixelShader* ShaderLibrary::getPixelShader(String pixelShaderName)
{
	if (this->activePixelShaders[pixelShaderName] == NULL) {
		std::cout << "Pixel shader " << pixelShaderName.c_str() << " not found. Have you initialized it? \n";
	}
	return this->activePixelShaders[pixelShaderName];
}

ShaderLibrary::ShaderLibrary()
{
	//initialize and load all shaders for use
	GraphicsEngine* graphEngine = GraphicsEngine::getInstance();

	ShaderNames shaderNames;
	ShaderData shaderData;
	graphEngine->compileVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME.c_str(), "main", &shaderData.shaderByteCode, &shaderData.sizeShader);
	this->activeVertexShaders[shaderNames.BASE_VERTEX_SHADER_NAME] = graphEngine->createVertexShader(shaderData.shaderByteCode, shaderData.sizeShader);

	graphEngine->compilePixelShader(shaderNames.BASE_PIXEL_SHADER_NAME.c_str(), "main", &shaderData.shaderByteCode, &shaderData.sizeShader);
	this->activePixelShaders[shaderNames.BASE_PIXEL_SHADER_NAME] = graphEngine->createPixelShader(shaderData.shaderByteCode, shaderData.sizeShader);

	graphEngine->compileVertexShader(shaderNames.TEXTURED_VERTEX_SHADER_NAME.c_str(), "main", &shaderData.shaderByteCode, &shaderData.sizeShader);
	this->activeVertexShaders[shaderNames.TEXTURED_VERTEX_SHADER_NAME] = graphEngine->createVertexShader(shaderData.shaderByteCode, shaderData.sizeShader);

	graphEngine->compilePixelShader(shaderNames.TEXTURED_PIXEL_SHADER_NAME.c_str(), "main", &shaderData.shaderByteCode, &shaderData.sizeShader);
	this->activeVertexShaders[shaderNames.TEXTURED_PIXEL_SHADER_NAME] = graphEngine->createVertexShader(shaderData.shaderByteCode, shaderData.sizeShader);

	std::cout << "Shader library initialized. \n";
}

ShaderLibrary::~ShaderLibrary()
{
	ShaderNames shaderNames;
	this->activeVertexShaders[shaderNames.BASE_VERTEX_SHADER_NAME]->release();
	this->activePixelShaders[shaderNames.BASE_PIXEL_SHADER_NAME]->release();

	this->activeVertexShaders.clear();
	this->activePixelShaders.clear();
}
