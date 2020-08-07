#pragma once
class Vector3D
{
public:
	Vector3D();
	~Vector3D();

	Vector3D(float x, float y, float z);
	Vector3D(const Vector3D* vector);
	
	float getX() const;
	float getY() const;
	float getZ() const;
	
	Vector3D operator *(float num);
	Vector3D operator +(Vector3D vec);

	static Vector3D lerp(const Vector3D start, const Vector3D end, float delta);
	static Vector3D zeros();
	static Vector3D ones();

private: 
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

