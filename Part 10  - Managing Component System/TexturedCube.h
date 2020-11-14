#pragma once
#include "Cube.h"
#include "Vector2D.h"

class TextureRenderer;
class TexturedCube :    public Cube
{
public:
	TexturedCube(String name);
	~TexturedCube() override;

	void draw(int width, int height) override;
	void setRenderer(ABaseRenderer* renderer) override;
	ABaseRenderer* getRenderer() const override;
private:
	struct Vertex {
		Vector3D position;
		Vector2D texCoord;
	};

	TextureRenderer* renderer;
};

