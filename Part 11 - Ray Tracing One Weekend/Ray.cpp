#include "Ray.h"

Ray::Ray()
{
}

Ray::Ray(const Point3D origin, const Vector3D direction)
{
	this->origin = origin;
	this->direction = direction;
}

Point3D Ray::getOrigin() const
{
	return this->origin;
}

Vector3D Ray::getDirection() const
{
	return this->direction;
}

Point3D Ray::at(float t) const
{
	return this->origin + this->direction * t;
}
