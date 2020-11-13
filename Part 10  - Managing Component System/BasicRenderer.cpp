#include "BasicRenderer.h"
#include "DeviceContext.h"
#include "GraphicsEngine.h"
#include "ShaderLibrary.h"

BasicRenderer::BasicRenderer() : ABaseRenderer(BasicRenderer::TEXTURE)
{
	
}

BasicRenderer::~BasicRenderer()
{
}

void BasicRenderer::configureDeviceContext() const
{
	ShaderNames shaderNames;
	DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateContext();
	deviceContext->setRenderConfig(ShaderLibrary::getInstance()->getVertexShader(shaderNames.BASE_VERTEX_SHADER_NAME), ShaderLibrary::getInstance()->getPixelShader(shaderNames.BASE_PIXEL_SHADER_NAME));
}
