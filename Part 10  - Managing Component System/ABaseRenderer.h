#pragma once
#include "TextureManager.h"

/// <summary>
/// Base class for object rendering component. This is similar to mesh renderer in Unity or renderer component in Unreal.
/// TODO: Will become a material data soon.
/// </summary>
class ABaseRenderer
{
protected:
	typedef std::string String;

public:
	enum RendererType { DEFAULT = 0, TEXTURE = 1, MATERIAL = 2 };
	ABaseRenderer(RendererType rendererType);
	virtual ~ABaseRenderer();
	virtual void setMaterialPath(String path);
	String getMaterialPath() const;
	virtual void configureDeviceContext() const = 0;
	RendererType getRendererType();

protected:
	RendererType rendererType;
	String materialPath;
};

