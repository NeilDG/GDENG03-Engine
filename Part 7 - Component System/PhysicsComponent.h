#pragma once
#include "AComponent.h"
#include <reactphysics3d/reactphysics3d.h>

using namespace reactphysics3d;
class PhysicsComponent: public AComponent
{
public:
	PhysicsComponent(String name, ComponentType type, AGameObject* owner);
	~PhysicsComponent();
	void perform(float deltaTime) override;

private:
	float mass = 1000.0f; //in kilograms

	PhysicsCommon physicsCommon;
	RigidBody* rigidBody;
	BoxShape* boxShape;
	PhysicsWorld* world;
};

