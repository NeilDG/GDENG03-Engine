#pragma once
#include "AGameObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

class Cube :    public AGameObject
{
public:
	Cube(String name, bool skipInit = false);
	~Cube();

	void update(float deltaTime) override;
	virtual void draw(int width, int height) override;

protected:

	struct Vertex {
		Vector3D position;
		Vector3D color;
		Vector3D color2;
	};

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;
	ConstantBuffer* constantBuffer = NULL;
	float ticks = 0.0f;
};

