#pragma once
#include "Cube.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

//Plane implementation is just like cube with different scale
class Plane :    public Cube
{
public:
	Plane(String name);
	~Plane();

	void update(float deltaTime) override;
	void draw(int width, int height) override;

};

