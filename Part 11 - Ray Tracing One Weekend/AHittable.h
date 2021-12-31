#pragma once
#include "Ray.h"

class HitRecord
{
public:
	Point3D p;
	Vector3D normal;
	float t;
	bool isFrontFace;

	inline void setFaceNormal(const Ray r, const Vector3D outwardNormal)
	{
		this->isFrontFace = Vector3D::dot(r.getDirection(), outwardNormal) < 0;
		this->normal = isFrontFace ? outwardNormal : -outwardNormal;
	}
};

class AHittable
{
public:
	virtual bool hit(const Ray r, float tMin, float tMax, HitRecord& rec) const = 0;
};
