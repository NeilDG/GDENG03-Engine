#include "EditorAction.h"

EditorAction::EditorAction(AGameObject* gameObject)
{
	//capture object state
	this->objectName = gameObject->getName();
	this->localPosition = gameObject->getLocalPosition();
	this->localRotation = gameObject->getLocalRotationRaw();
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

Vector3D EditorAction::getStoredOrientation()
{
	return this->localRotation;
}

Matrix4x4 EditorAction::getStoredMatrix()
{
	return this->localMatrix;
}
