#pragma once
#include "AHittable.h"
class Sphere : public AHittable
{
public:
	Sphere();
	Sphere(Point3D center, float radius);

	virtual bool hit(const Ray r, float tMin, float tMax, HitRecord& rec) const override;

	Point3D getCenter();
	float getRadius();

private:
	Point3D center;
	float radius;
	
};

