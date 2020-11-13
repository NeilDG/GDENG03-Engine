#pragma once
#include "Cube.h"
#include "Vector2D.h"

class TextureRenderer;
class TexturedCube :    public Cube
{
public:
	TexturedCube(String name);
	~TexturedCube() override;

	void attachRenderer(TextureRenderer* renderer);
	void draw(int width, int height) override;
	TextureRenderer* getRenderer() const;
private:
	struct Vertex {
		Vector3D position;
		Vector2D texCoord;
	};

	TextureRenderer* renderer;
};

