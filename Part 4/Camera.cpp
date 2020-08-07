#include "Camera.h"
#include "InputSystem.h"

Camera::Camera(string name): AGameObject(name)
{
	this->updateViewMatrix();
}

Camera::~Camera()
{
}

void Camera::update(float deltaTime)
{
	Vector3D localRot = this->getLocalRotation();
	float x = localRot.getX();
	float y = localRot.getY();
	float z = localRot.getZ();

	if (InputSystem::getInstance()->isKeyDown('D')) {
		y += deltaTime;
		this->setRotation(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('A')) {
		y -= deltaTime;
		this->setRotation(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('W')) {
		x += deltaTime;
		this->setRotation(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('S')) {
		x -= deltaTime;
		this->setRotation(x, y, z);
		this->updateViewMatrix();
	}
}

Matrix4x4 Camera::getViewMatrix()
{
	return this->localMatrix;
}

void Camera::updateViewMatrix()
{
	Matrix4x4 worldCam; worldCam.setIdentity();
	Matrix4x4 temp; temp.setIdentity();

	Vector3D localRot = this->getLocalRotation();

	temp.setRotationX(localRot.getX());
	worldCam = worldCam.multiplyTo(temp);

	temp.setRotationY(localRot.getY());
	worldCam = worldCam.multiplyTo(temp);

	temp.setTranslation(Vector3D(0.0f, 0.0f, -2.0f));
	worldCam = worldCam.multiplyTo(temp);
	worldCam.debugPrint(); std::cout << "\n";
	worldCam.getInverse();
	worldCam.debugPrint(); std::cout << "\n";
	this->localMatrix = worldCam;
}
