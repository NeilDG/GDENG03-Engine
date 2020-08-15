#include "PhysicsComponent.h"
#include <iostream>

PhysicsComponent::PhysicsComponent(String name, ComponentType type, AGameObject* owner): AComponent(name, type, owner)
{

}

void PhysicsComponent::perform(float deltaTime)
{
	std::cout << "Performing physics update: " << deltaTime << "\n";
}
