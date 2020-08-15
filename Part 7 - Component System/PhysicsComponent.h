#pragma once
#include "AComponent.h"

class PhysicsComponent: public AComponent
{
public:
	PhysicsComponent(String name, ComponentType type, AGameObject* owner);
	~PhysicsComponent();
	void perform(float deltaTime) override;

private:
	float mass = 1.0f; //in kilograms
};

