#pragma once
#include <memory>
#include "Vector3D.h"
#include <iostream>

class Matrix4x4
{
public:
	Matrix4x4();
	~Matrix4x4();
	void setIdentity();
	void setTranslation(const Vector3D translation);
	void setScale(const Vector3D scale);
	void setOrthoLH(float width, float height, float near_plane, float far_plane);
	Matrix4x4 multiplyTo(Matrix4x4 matrix);

	void debugPrint();
	
private:
	void matrixInitialize();

	float matrix[4][4] = {};
};

