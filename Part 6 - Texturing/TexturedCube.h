#pragma once
#include "Cube.h"
#include "Vector2D.h"

class TexturedCube :    public Cube
{
public:
	TexturedCube(String name);
	~TexturedCube();

	void draw(int width, int height) override;
private:
	struct Vertex {
		Vector3D position;
		Vector2D texCoord;
	};
};

