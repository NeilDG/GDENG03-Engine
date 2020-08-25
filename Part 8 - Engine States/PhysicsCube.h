#pragma once
#include "Cube.h"
#include "PhysicsComponent.h"

class PhysicsCube :    public Cube
{
public:
	PhysicsCube(String name, bool skipInit = false);
	~PhysicsCube();

	void update(float deltaTime) override;
	void draw(int width, int height) override;

	void saveEditState() override;
	void restoreEditState() override;

protected:
	float mass = 3.0f;
	PhysicsComponent* componentAttached;

	void awake() override;

};

