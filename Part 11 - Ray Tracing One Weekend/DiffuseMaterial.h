#pragma once
#include "AMaterial.h"
class DiffuseMaterial : public AMaterial
{
public:
	DiffuseMaterial(const Color a);
	bool scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;

private:
	Color albedo;
};

