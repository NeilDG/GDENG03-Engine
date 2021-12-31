#include "Vector3D.h"
#include <cmath>

Vector3D::Vector3D()
{
	
}

Vector3D::Vector3D(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::Vector3D(const Vector3D* vector)
{
	this->x = vector->x;
	this->y = vector->y;
	this->z = vector->z;
}

float Vector3D::getX() const
{
	return this->x;
}

float Vector3D::getY() const
{
	return this->y;
}

float Vector3D::getZ() const
{
	return this->z;
}

Vector3D Vector3D::operator*(const float num) const
{
	return Vector3D(this->x * num, this->y * num, this->z * num);
}

Vector3D Vector3D::operator*(const Vector3D vec) const
{
	return Vector3D(this->x * vec.x, this->y * vec.y, this->z * vec.z);
}

Vector3D Vector3D::operator+(const Vector3D vec) const
{
	return Vector3D(this->x + vec.x, this->y + vec.y, this->z + vec.z);
}

Vector3D Vector3D::operator+(float num) const
{
	return Vector3D(this->x + num, this->y + num, this->z + num);
}

Vector3D Vector3D::operator-(float num) const
{
	return Vector3D(this->x - num, this->y - num, this->z - num);
}

Vector3D Vector3D::operator-() const
{
	return Vector3D(-this->x, - this->y, -this->z);
}

Vector3D Vector3D::operator-(const Vector3D vec) const
{
	return Vector3D(this->x - vec.x, this->y - vec.y, this->z - vec.z);
}

Vector3D Vector3D::operator/(const Vector3D vec) const
{
	return Vector3D(this->x / vec.x, this->y / vec.y, this->z / vec.z);
}

Vector3D Vector3D::operator/(const float num) const
{
	return Vector3D(this->x / num, this->y / num, this->z / num);
}

float Vector3D::length() const
{
	return sqrt(this->lengthSquared());
}

float Vector3D::lengthSquared() const
{
	return this->x * this->x + this->y * this->y + this->z * this->z;
}

float Vector3D::dot(const Vector3D u, const Vector3D v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector3D Vector3D::cross(const Vector3D u, const Vector3D v)
{
	return Vector3D(u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x);
}

Vector3D Vector3D::unitVector(const Vector3D v)
{
	return v / v.length();
}

Vector3D Vector3D::lerp(const Vector3D start, const Vector3D end, float delta)
{
	Vector3D v;
	v.x = start.x * (1.0f - delta) + end.x * (delta);
	v.y = start.y * (1.0f - delta) + end.y * (delta);
	v.z = start.z * (1.0f - delta) + end.z * (delta);

	return v;
}

Vector3D Vector3D::zeros()
{
	return Vector3D(0.0f, 0.0f, 0.0f);
}

Vector3D Vector3D::ones()
{
	return Vector3D(1.0f, 1.0f, 1.0f);
}

Vector3D::~Vector3D()
{
}
