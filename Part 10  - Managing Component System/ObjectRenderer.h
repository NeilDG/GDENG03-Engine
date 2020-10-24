#pragma once
#include "TextureManager.h"

/// <summary>
/// Takes care of the rendering of object, based on material attached. Standalone\
/// class independent from component system.
/// TODO: Will become a material data soon.
/// </summary>
class ObjectRenderer
{
protected:
	typedef std::string String;
	
public:
	void setRenderer(String path);
	String getMaterialPath() const;
	Texture* getTexture() const;

private:
	String materialPath;
	Texture* texture;
};

