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

	//Vector3 position = Vector3(pos.getX(), pos.getY(), pos.getZ());
	//Quaternion orientation = Quaternion::identity();
	//Transform transform = Transform(position, orientation);
	Transform transform; transform.setFromOpenGL(this->getOwner()->getPhysicsLocalMatrix());
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
	if (this->rigidBody->getType() == BodyType::DYNAMIC) {
		const Transform transform = this->rigidBody->getTransform();
		float matrix[16];
		transform.getOpenGLMatrix(matrix);

		//this->getOwner()->setPosition(transform.getPosition().x, transform.getPosition().y, transform.getPosition().z);
		this->getOwner()->setLocalMatrix(matrix);
	}
}

RigidBody* PhysicsComponent::getRigidBody()
{
	return this->rigidBody;
}
