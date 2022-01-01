#pragma once
#include "AMaterial.h"
class DielectricMaterial : public AMaterial
{
public:
	DielectricMaterial(float refractionIndex);
	bool scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;
private:
	float calculateReflectance(float cosine, float refIdx) const;
	float refractionIndex;
};

