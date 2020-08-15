#pragma once
#include "Cube.h"
class PhysicsCube :    public Cube
{
	PhysicsCube(String name, bool skipInit = false);
	~PhysicsCube();

	void update(float deltaTime) override;
	void draw(int width, int height) override;

protected:
	float mass = 3.0f;
};

