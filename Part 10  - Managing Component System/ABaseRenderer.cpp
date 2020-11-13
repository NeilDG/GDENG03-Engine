#include "ABaseRenderer.h"

ABaseRenderer::ABaseRenderer(RendererType rendererType)
{
	this->rendererType = rendererType;
}

ABaseRenderer::~ABaseRenderer()
{
	
}

void ABaseRenderer::setMaterialPath(String path)
{
	this->materialPath = path;
}

ABaseRenderer::String ABaseRenderer::getMaterialPath() const
{
	return this->materialPath;
}

ABaseRenderer::RendererType ABaseRenderer::getRendererType()
{
	return this->rendererType;
}
