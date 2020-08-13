#pragma once
#include "Cube.h"
#include "Vector2D.h"

class TexturedCube :    public Cube
{
public:
	TexturedCube(String name, void* shaderByteCode, size_t sizeShader);
	~TexturedCube();
private:
	struct Vertex {
		Vector3D position;
		Vector2D texCoord;
	};
};

