#include "PhysicsCube.h"

PhysicsCube::PhysicsCube(String name, bool skipInit): Cube(name, skipInit)
{

}

PhysicsCube::~PhysicsCube()
{
	Cube::~Cube();
}

void PhysicsCube::update(float deltaTime)
{
	Cube::update(deltaTime);
}

void PhysicsCube::draw(int width, int height)
{
	Cube::draw(width, height);
}
