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
	
	Vector3D operator *(const float num) const;
	Vector3D operator *(const Vector3D vec) const;
	Vector3D operator +(const Vector3D vec) const;
	Vector3D operator +(float num) const;
	Vector3D operator -(const Vector3D vec) const;
	Vector3D operator -(float num) const;
	Vector3D operator -() const;
	Vector3D operator /(const Vector3D vec) const;
	Vector3D operator /(const float num) const;

	float length() const;
	float lengthSquared() const;
	bool nearZero() const;

	static Vector3D lerp(const Vector3D start, const Vector3D end, float delta);
	static Vector3D zeros();
	static Vector3D ones();
	static float dot(const Vector3D u, const Vector3D v);
	static Vector3D cross(const Vector3D u, const Vector3D v);
	static Vector3D unitVector(const Vector3D v);
	static Vector3D random();
	static Vector3D random(float min, float max);
	static Vector3D randomUnitVector();
	static Vector3D randomInHemisphere(const Vector3D normal);
	static Vector3D randomInUnitSphere();
	static Vector3D randomInUnitDisk();
	
	static Vector3D reflect(const Vector3D v, const Vector3D n);
	static Vector3D refract(const Vector3D uv, const Vector3D n, float etaiOverEtat);
	
	

private: 
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

using Point3D = Vector3D;
using Color = Vector3D;

