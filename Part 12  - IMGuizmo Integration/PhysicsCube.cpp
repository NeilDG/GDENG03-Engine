#include "PhysicsCube.h"


PhysicsCube::PhysicsCube(String name, bool skipInit): Cube(name, skipInit)
{
	this->objectType = PrimitiveType::PHYSICS_CUBE;
	this->setPosition(0.0f, 5.0f, 0.0f);
	this->setScale(1.0f, 1.0f, 1.0f);
}

void PhysicsCube::awake()
{
	AGameObject::awake();
	this->componentAttached = new PhysicsComponent("PhysicsComponent_" + this->name, this);
	this->attachComponent(this->componentAttached);
}

PhysicsCube::~PhysicsCube()
{
	this->detachComponent(this->componentAttached);
	delete this->componentAttached;
	Cube::~Cube();
}

void PhysicsCube::update(float deltaTime)
{
	Cube::update(deltaTime);
	//std::cout << "My object is updating: " << this->name << "\n";
}

void PhysicsCube::draw(int width, int height)
{
	Cube::draw(width, height);
}

void PhysicsCube::saveEditState()
{
	AGameObject::saveEditState();
}

void PhysicsCube::restoreEditState()
{
	AGameObject::restoreEditState();
	this->detachComponent(this->componentAttached);
	delete this->componentAttached;

	//also restore physics by redeclaring component
	this->componentAttached = new PhysicsComponent("PhysicsComponent_" + this->name, this);
	this->attachComponent(this->componentAttached);
}
