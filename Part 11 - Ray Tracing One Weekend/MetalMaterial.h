#pragma once
#include "AMaterial.h"
class MetalMaterial : public AMaterial
{
public:
	MetalMaterial(const Color a, float f);

	bool scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;

private:
	Color albedo;
	float fuzziness;
};

