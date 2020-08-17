#include "PhysicsComponent.h"
#include <iostream>
#include "BaseComponentSystem.h"
#include "PhysicsSystem.h"
#include "AGameObject.h"
#include "Vector3D.h"

PhysicsComponent::PhysicsComponent(String name, AGameObject* owner): AComponent(name, AComponent::ComponentType::Physics, owner)
{
	//whenever a new physics component is initialized. Register to physics system
	BaseComponentSystem::getInstance()->getPhysicsSystem()->registerComponent(this);
	PhysicsCommon* physicsCommon = BaseComponentSystem::getInstance()->getPhysicsSystem()->getPhysicsCommon();
	PhysicsWorld* physicsWorld = BaseComponentSystem::getInstance()->getPhysicsSystem()->getPhysicsWorld();
	
	// Create a rigid body in the world
	Vector3D pos = this->getOwner()->getLocalPosition();
	Vector3D scale = this->getOwner()->getLocalScale();
	Vector3D rot = this->getOwner()->getLocalRotation();

	Vector3 position = Vector3(pos.getX(), pos.getY(), pos.getZ());
	Quaternion orientation = Quaternion::fromEulerAngles(rot.getX(), rot.getY(), rot.getZ());
	Transform transform = Transform(position, orientation);
	
	BoxShape* boxShape = physicsCommon->createBoxShape(Vector3(scale.getX()/2, scale.getY()/2, scale.getZ()/2)); //half extent
	this->rigidBody = physicsWorld->createRigidBody(transform);
	this->rigidBody->addCollider(boxShape, transform);
	this->rigidBody->updateMassPropertiesFromColliders();
	this->rigidBody->setMass(this->mass);
	this->rigidBody->setType(BodyType::DYNAMIC);

	// Force vector (in Newton )
	//Vector3 force(0.0f, 4000.0f, 0.0f);
	//this->rigidBody->applyTorque(force);
}

PhysicsComponent::~PhysicsComponent()
{
	AComponent::~AComponent();
	BaseComponentSystem::getInstance()->getPhysicsSystem()->unregisterComponent(this);
}

void PhysicsComponent::perform(float deltaTime)
{
	const Transform transform = this->rigidBody->getTransform();
	const Vector3 position = transform.getPosition();

	const Quaternion rot = transform.getOrientation();
	const Vector3 convertedRot = rot.getVectorV();

	this->getOwner()->setPosition(position.x, position.y, position.z);
	this->getOwner()->setRotation(convertedRot.x, convertedRot.y, convertedRot.z);
}

RigidBody* PhysicsComponent::getRigidBody()
{
	return this->rigidBody;
}
