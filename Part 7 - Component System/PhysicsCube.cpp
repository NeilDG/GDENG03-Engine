#include "PhysicsCube.h"
#include "PhysicsComponent.h"

PhysicsCube::PhysicsCube(String name, bool skipInit): Cube(name, skipInit)
{
	this->setPosition(0.0f, 0.0f, 0.0f);
	this->setScale(6.0f, 6.0f, 1.0f);
	this->updateLocalMatrix();
	this->attachComponent(new PhysicsComponent("PhysicsComponent", this));
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
