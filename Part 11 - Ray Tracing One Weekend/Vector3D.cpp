#include "Vector3D.h"
#include <cmath>
#include "MathUtils.h"

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

bool Vector3D::nearZero() const
{
	float s = 1e-8;
	return(fabs(this->x) < s && fabs(this->y) < s && fabs(this->z) < s);
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

Vector3D Vector3D::random()
{
	float x = MathUtils::randomFloat();
	float y = MathUtils::randomFloat();
	float z = MathUtils::randomFloat();
	return Vector3D(x, y, z);
}

Vector3D Vector3D::random(float min, float max)
{
	float x = MathUtils::randomFloat(min, max);
	float y = MathUtils::randomFloat(min, max);
	float z = MathUtils::randomFloat(min, max);

	return Vector3D(x, y, z);
}

Vector3D Vector3D::randomUnitVector()
{
	return unitVector(randomInUnitSphere());
}

Vector3D Vector3D::randomInUnitSphere()
{
	while(true)
	{
		Vector3D p = Vector3D::random(-1.0f, 1.0f);
		if (p.lengthSquared() >= 1.0f) continue;
		return p;
	}
}

Vector3D Vector3D::randomInUnitDisk()
{
	while(true)
	{
		Vector3D p = Vector3D(MathUtils::randomFloat(-1.0f, 1.0f), MathUtils::randomFloat(-1.0f, 1.0f), 0.0f);
		if (p.lengthSquared() >= 1.0f) continue;
		return p;
	}
}

Vector3D Vector3D::randomInHemisphere(const Vector3D normal)
{
	Vector3D inUnitSphere = randomInUnitSphere();
	if(dot(inUnitSphere, normal) > 0.0f)
	{
		return inUnitSphere;
	}
	else
	{
		return -inUnitSphere;
	}
}

Vector3D Vector3D::reflect(const Vector3D v, const Vector3D n)
{
	return v - n * 2 * dot(v, n);
}

Vector3D Vector3D::refract(const Vector3D uv, const Vector3D n, float etaiOverEtat)
{
	float cosTheta = fmin(dot(-uv, n), 1.0f);
	Vector3D rPerpendicular = (uv + n * cosTheta) * etaiOverEtat;
	Vector3D rParallel = n * -sqrt(fabs(1.0f - rPerpendicular.lengthSquared()));
	return rPerpendicular + rParallel;
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
