#pragma once
#include "AGameObject.h"
#include "InputListener.h"

class Camera :    public AGameObject, public InputListener
{
public:
	Camera(String name);
	~Camera();

	void update(float deltaTime) override;
	Matrix4x4 getViewMatrix();
	Matrix4x4 getProjectionMatrix();

	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point deltaPos) override;
	virtual void onLeftMouseDown(const Point deltaPos) override;
	virtual void onLeftMouseUp(const Point deltaPos) override;
	virtual void onRightMouseDown(const Point deltaPos) override;
	virtual void onRightMouseUp(const Point deltaPos) override;


private:
	void updateViewMatrix();

	const float FOV_DEGREES_DEFAULT = 60.0f;
	const float ZNEAR_DEFAULT = 0.1f;
	const float ZFAR_DEFAULT = 1000.0f;
	
	float ticks = 0.0f;
	float mouseDown = false;

	float fovDegrees;
	float zNear;
	float zFar;

	//float forwardDirection = 0.0f;
	//Matrix4x4 worldCameraMatrix;
};

