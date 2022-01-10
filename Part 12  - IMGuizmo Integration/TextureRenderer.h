#pragma once
#include "TextureManager.h"
#include "ABaseRenderer.h"

/// <summary>
/// Takes care of the rendering of object, based on material attached. Standalone
/// class independent from component system.
/// TODO: Will become a material data soon.
/// </summary>

class TextureRenderer: public ABaseRenderer
{	
public:
	TextureRenderer();
	~TextureRenderer();
	void setMaterialPath(String path) override;
	String getMaterialPath() const;
	Texture* getTexture() const;
	void configureDeviceContext() const; //updates device context. This must be called before drawing the object.

private:
	Texture* texture;
};

