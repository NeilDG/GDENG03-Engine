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
	void* shaderByteCode = NULL;
	size_t sizeShader = 0;

	ShaderNames shaderNames;
	graphEngine->compileVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	this->activeVertexShaders[shaderNames.BASE_VERTEX_SHADER_NAME] = graphEngine->createVertexShader(shaderByteCode, sizeShader);
	graphEngine->releaseCompiledShader();

	graphEngine->compilePixelShader(shaderNames.BASE_PIXEL_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	this->activePixelShaders[shaderNames.BASE_PIXEL_SHADER_NAME] = graphEngine->createPixelShader(shaderByteCode, sizeShader);
	graphEngine->releaseCompiledShader();

	graphEngine->compileVertexShader(shaderNames.TEXTURED_VERTEX_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	this->activeVertexShaders[shaderNames.TEXTURED_VERTEX_SHADER_NAME] = graphEngine->createVertexShader(shaderByteCode, sizeShader);

	graphEngine->compilePixelShader(shaderNames.TEXTURED_PIXEL_SHADER_NAME.c_str(), "main", &shaderByteCode, &sizeShader);
	this->activeVertexShaders[shaderNames.BASE_PIXEL_SHADER_NAME] = graphEngine->createVertexShader(shaderByteCode, sizeShader);
}

ShaderLibrary::~ShaderLibrary()
{
	ShaderNames shaderNames;
	this->activeVertexShaders[shaderNames.BASE_VERTEX_SHADER_NAME]->release();
	this->activePixelShaders[shaderNames.BASE_PIXEL_SHADER_NAME]->release();

	this->activeVertexShaders.clear();
	this->activePixelShaders.clear();
}
