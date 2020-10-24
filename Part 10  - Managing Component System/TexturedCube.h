#pragma once
#include "Cube.h"
#include "Vector2D.h"

class ObjectRenderer;
class TexturedCube :    public Cube
{
public:
	TexturedCube(String name);
	~TexturedCube() override;

	void attachRenderer(ObjectRenderer* renderer);
	void draw(int width, int height) override;
	ObjectRenderer* getRenderer() const;
private:
	struct Vertex {
		Vector3D position;
		Vector2D texCoord;
	};

	ObjectRenderer* renderer;
};

