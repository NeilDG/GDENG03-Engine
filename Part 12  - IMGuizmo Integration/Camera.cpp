#include "Camera.h"
#include "InputSystem.h"
#include "ImGuizmo.h"
#include "UIManager.h"

Camera::Camera(String name): AGameObject(name, PrimitiveType::CAMERA)
{
	this->fovDegrees = FOV_DEGREES_DEFAULT;
	this->zNear = ZNEAR_DEFAULT;
	this->zFar = ZFAR_DEFAULT;
	
	this->setPosition(0.0f, 0.0f, -4.0f);
	//this->worldCameraMatrix.setTranslation(this->getLocalPosition());
	this->updateViewMatrix();
	InputSystem::getInstance()->addListener(this);
}

Camera::~Camera()
{
	InputSystem::getInstance()->removeListener(this);
}

void Camera::update(float deltaTime)
{
	Vector3D localPos = this->getLocalPosition();
	float x = localPos.getX();
	float y = localPos.getY();
	float z = localPos.getZ();
	float moveSpeed = 10.0f;

	if (InputSystem::getInstance()->isKeyDown('W')) {
		if (this->mouseDown) {
			y += deltaTime * moveSpeed;
		}
		else {
			z += deltaTime * moveSpeed;
		}
		
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('S')) {
		if (this->mouseDown) {
			y -= deltaTime * moveSpeed;
		}
		else {
			z -= deltaTime * moveSpeed;
		}
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('A')) {
		x += deltaTime * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}
	else if (InputSystem::getInstance()->isKeyDown('D')) {
		x -= deltaTime * moveSpeed;
		this->setPosition(x, y, z);
		this->updateViewMatrix();
	}
}

Matrix4x4 Camera::getViewMatrix()
{
	return this->localMatrix;
}

Matrix4x4 Camera::getProjectionMatrix()
{
	int width = UIManager::WINDOW_WIDTH;
	int height = UIManager::WINDOW_HEIGHT;
	float aspectRatio = (float)width / (float)height;
	float fov = this->fovDegrees * 3.141592f / 180.0f;

	Matrix4x4 projectionMatrix;
	projectionMatrix.setPerspectiveFovLH(fov, aspectRatio, this->zNear, this->zFar);

	return projectionMatrix;
}

void Camera::onKeyDown(int key)
{
}

void Camera::onKeyUp(int key)
{
}

void Camera::onMouseMove(const Point deltaPos)
{
	if (this->mouseDown) {
		Vector3D localRot = this->getLocalRotation();
		float x = localRot.getX();
		float y = localRot.getY();
		float z = localRot.getZ();

		float speed = 0.005f;
		x += deltaPos.getY() * speed;
		y += deltaPos.getX() * speed;

		this->setRotation(x, y, z);
		this->updateViewMatrix();

		std::cout << " Local rot: " << this->getLocalRotation().getX() << " " << this->getLocalRotation().getY() << " " << this->getLocalRotation().getZ() << "\n";
	}
}

void Camera::onLeftMouseDown(const Point deltaPos)
{
}

void Camera::onLeftMouseUp(const Point deltaPos)
{
}

void Camera::onRightMouseDown(const Point deltaPos)
{
	this->mouseDown = true;
}

void Camera::onRightMouseUp(const Point deltaPos)
{
	this->mouseDown = false;
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

	temp.setTranslation(this->getLocalPosition());
	worldCam = worldCam.multiplyTo(temp);

	/*Vector3D cameraPos = this->worldCameraMatrix.getTranslation() + (worldCam.getZDirection() * (this->forwardDirection * 0.01f));
	std::cout << "Camera pos: " << cameraPos.getX() << " " << cameraPos.getY() << " " << cameraPos.getZ() << "\n";
	temp.setTranslation(cameraPos);
	worldCam = worldCam.multiplyTo(temp);
	this->worldCameraMatrix = worldCam;*/

	worldCam.getInverse();
	this->localMatrix = worldCam;
}
