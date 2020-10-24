#include "ObjectRenderer.h"
#include "ShaderLibrary.h"
#include "DeviceContext.h"
#include "GraphicsEngine.h"

void ObjectRenderer::setRenderer(String path)
{
	ShaderNames shaderNames;
	DeviceContext* deviceContext = GraphicsEngine::getInstance()->getImmediateContext();
	
	//convert to wchar format
	this->materialPath = path;
	String textureString = this->materialPath;
	std::wstring widestr = std::wstring(textureString.begin(), textureString.end());
	const wchar_t* texturePath = widestr.c_str();

	this->texture = static_cast<Texture*>(TextureManager::getInstance()->createTextureFromFile(texturePath));
}

ObjectRenderer::String ObjectRenderer::getMaterialPath() const
{
	return this->materialPath;
}

Texture* ObjectRenderer::getTexture() const
{
	return this->texture;
}
