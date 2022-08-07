#pragma once
#include <memory>
#include "Vector3D.h"
#include "Vector4D.h"
#include <iostream>

class Matrix4x4
{
public:
	Matrix4x4();
	~Matrix4x4();
	void setIdentity();
	void setMatrix(const Matrix4x4 matrix);
	void setMatrix(float matrix[4][4]);
	void setTranslation(const Vector3D translation);
	void setScale(const Vector3D scale);
	void setRotationX(float x);
	void setRotationY(float y);
	void setRotationZ(float z);

	void setPerspectiveFovLH(float fov, float aspect, float znear, float zfar);
	void setOrthoLH(float width, float height, float near_plane, float far_plane);
	void setInverse();

	Matrix4x4 multiplyTo(Matrix4x4 matrix);
	Vector3D getZDirection();
	Vector3D getXDirection();
	Vector3D getTranslation();

	void debugPrint();
	float* getMatrix();
	
private:
	void matrixInitialize();
	float determinant();

	float matrix[4][4] = {};
};

