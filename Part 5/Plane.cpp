#include "Plane.h"

Plane::Plane(string name, void* shaderByteCode, size_t sizeShader): Cube(name, shaderByteCode, sizeShader)
{
	this->setScale(8.0f, 8.0f, 0.1f);
	this->setRotation(190.0f, 0.0f, 0.0f);
}

Plane::~Plane()
{
}

void Plane::update(float deltaTime)
{
	Cube::update(deltaTime);
}

void Plane::draw(int width, int height, VertexShader* vertexShader, PixelShader* pixelShader)
{
	Cube::draw(width, height, vertexShader, pixelShader);
}
