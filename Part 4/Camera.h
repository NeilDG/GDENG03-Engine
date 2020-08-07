#pragma once
#include "AGameObject.h"
#include "InputListener.h"

class Camera :    public AGameObject, public InputListener
{
public:
	Camera(string name);
	~Camera();

	void update(float deltaTime) override;
	Matrix4x4 getViewMatrix();

	virtual void onKeyDown(int key) override;
	virtual void onKeyUp(int key) override;
	virtual void onMouseMove(const Point deltaPos) override;
	virtual void onLeftMouseDown(const Point deltaPos) override;
	virtual void onLeftMouseUp(const Point deltaPos) override;
	virtual void onRightMouseDown(const Point deltaPos) override;
	virtual void onRightMouseUp(const Point deltaPos) override;

private:
	void updateViewMatrix();

	float ticks = 0.0f;
	float speed = 10.0f;
	float mouseDown = false;

	//float forwardDirection = 0.0f;
	//Matrix4x4 worldCameraMatrix;
};

