#pragma once
#include "AGameObject.h"
class Camera :    public AGameObject
{
public:
	Camera(string name);
	~Camera();

	void update(float deltaTime) override;
	Matrix4x4 getViewMatrix();

private:
	float ticks = 0.0f;
	float speed = 10.0f;

	void updateViewMatrix();
};

