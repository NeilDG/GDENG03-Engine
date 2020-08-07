#pragma once
class Matrix4x4;
class Vector4D
{
public:
	Vector4D();
	~Vector4D();

	Vector4D(float x, float y, float z, float w);
	Vector4D(const Vector4D* vector);

	void cross(Vector4D v1, Vector4D v2, Vector4D v3);

	float getX();
	float getY();
	float getZ();
	float getW();

private:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	friend class Matrix4x4;
};

