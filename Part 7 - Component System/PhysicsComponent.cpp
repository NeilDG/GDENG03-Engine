#include "PhysicsComponent.h"
#include <iostream>
#include "BaseComponentSystem.h"
#include "PhysicsSystem.h"
#include "AGameObject.h"
#include "Vector3D.h"

PhysicsComponent::PhysicsComponent(String name, ComponentType type, AGameObject* owner): AComponent(name, type, owner)
{
	//whenever a new physics component is initialized. Register to physics system
	BaseComponentSystem::getInstance()->getPhysicsSystem()->registerComponent(this);

	// Create the world settings
	PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;
	settings.gravity = Vector3(0, -1.81, 0);

	this->world = this->physicsCommon.createPhysicsWorld(settings);
	std::cout << "Successfully created world. \n";

	// Create a rigid body in the world
	Vector3D pos = this->getOwner()->getLocalPosition();
	Vector3D scale = this->getOwner()->getLocalScale();

	Vector3 position = Vector3(pos.getX(), pos.getY(), pos.getZ());
	Quaternion orientation = Quaternion::identity();
	Transform transform = Transform(position, orientation);
	this->rigidBody = this->world->createRigidBody(transform);
	this->rigidBody->setMass(this->mass);
	this->boxShape = this->physicsCommon.createBoxShape(Vector3(1.0f, 1.0f, 1.0f)); //half extent
	this->rigidBody->addCollider(this->boxShape, transform);
	this->rigidBody->setType(BodyType::DYNAMIC);
	// Force vector (in Newton )
	Vector3 force(0.0f, 4000.0f, 0.0f);
	this->rigidBody->applyTorque(force);
}

PhysicsComponent::~PhysicsComponent()
{
	AComponent::~AComponent();
	BaseComponentSystem::getInstance()->getPhysicsSystem()->unregisterComponent(this);
}

void PhysicsComponent::perform(float deltaTime)
{
	this->world->update(deltaTime);

	const Transform transform = this->rigidBody->getTransform();
	const Vector3 position = transform.getPosition();
	std::cout << " Body Position : (" << position.x << "," <<
		position.y << ", " << position.z << ")" << std::endl;

	const Quaternion rot = transform.getOrientation();
	const Vector3 convertedRot = rot.getVectorV();
	std::cout << " Body Rotation : (" << convertedRot.x << "," <<
		convertedRot.y << ", " << convertedRot.z << ")" << std::endl;

	this->getOwner()->setPosition(position.x, position.y, position.z);
	this->getOwner()->setRotation(convertedRot.x, convertedRot.y, convertedRot.z);
}
