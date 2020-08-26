#include "EditorAction.h"

EditorAction::EditorAction(AGameObject* gameObject)
{
	//capture object state
	this->objectName = gameObject->getName();
	this->localPosition = gameObject->getLocalPosition();
	this->orientation = {};
	Vector3D rotation = gameObject->getLocalRotation();
	this->orientation.x = rotation.getX();
	this->orientation.y = rotation.getY();
	this->orientation.z = rotation.getZ();
	this->localScale = gameObject->getLocalScale();
	this->localMatrix = gameObject->getLocalMatrix();
}

EditorAction::~EditorAction()
{

}

AGameObject::String EditorAction::getOwnerName()
{
	return this->objectName;
}

Vector3D EditorAction::getStorePos()
{
	return this->localPosition;
}

Vector3D EditorAction::getStoredScale()
{
	return this->localScale;
}

AGameObject::AQuaternion EditorAction::getStoredOrientation()
{
	return this->orientation;
}

Matrix4x4 EditorAction::getStoredMatrix()
{
	return this->localMatrix;
}
