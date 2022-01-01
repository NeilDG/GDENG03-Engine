#pragma once
#include "AHittable.h"
#include "Vector3D.h"

class AMaterial
{
public:
	virtual bool scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

