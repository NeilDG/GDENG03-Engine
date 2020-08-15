#pragma once
#include "AComponent.h"

class PhysicsComponent: public AComponent
{
public:
	PhysicsComponent(String name, ComponentType type, AGameObject* owner);
	void perform(float deltaTime) override;
};

