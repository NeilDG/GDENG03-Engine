#pragma once
#include "Vector3D.h"

class Ray
{
	public:
		Ray();
		Ray(const Point3D origin, const Vector3D direction);

		Point3D getOrigin() const;
		Vector3D getDirection() const;

		Point3D at(float t) const;

	private:
		Point3D origin;
		Vector3D direction;
};

