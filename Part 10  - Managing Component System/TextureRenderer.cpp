#include "TextureRenderer.h"
#include "ShaderLibrary.h"
#include "DeviceContext.h"
#include "GraphicsEngine.h"

TextureRenderer::TextureRenderer(): ABaseRenderer(RendererType::TEXTURE)
{
	
}

TextureRenderer::~TextureRenderer()
{
	ABaseRenderer::~ABaseRenderer();
}

void TextureRenderer::setMaterialPath(String path)
{
	ABaseRenderer::setMaterialPath(path);
	//convert to wchar format	
	String textureString = this->materialPath;
	std::wstring widestr = std::wstring(textureString.begin(), textureString.end());
	const wchar_t* texturePath = widestr.c_str();

	this->texture = static_cast<Texture*>(TextureManager::getInstance()->createTextureFromFile(texturePath));
}

TextureRenderer::String TextureRenderer::getMaterialPath() const
{
	return this->materialPath;
}

Texture* TextureRenderer::getTexture() const
{
	return this->texture;
}

void TextureRenderer::configureDeviceContext() const
{
	ShaderNames shaderNames;
	DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateContext();
	deviceContext->setTexture(this->getTexture());
	deviceContext->setRenderConfig(ShaderLibrary::getInstance()->getVertexShader(shaderNames.TEXTURED_VERTEX_SHADER_NAME), ShaderLibrary::getInstance()->getPixelShader(shaderNames.TEXTURED_PIXEL_SHADER_NAME));

}
