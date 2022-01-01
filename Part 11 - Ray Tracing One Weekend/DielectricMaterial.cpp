#include "DielectricMaterial.h"

DielectricMaterial::DielectricMaterial(float refractionIndex)
{
	this->refractionIndex = refractionIndex;
}

bool DielectricMaterial::scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	attenuation = Color(1.0f, 1.0f, 1.0f);
	float refractionRatio = rec.isFrontFace ? (1.0f / this->refractionIndex) : this->refractionIndex;

	Vector3D unitDirection = Vector3D::unitVector(rIn.getDirection());
	float cosTheta = fmin(Vector3D::dot(-unitDirection, rec.normal), 1.0f);
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	bool cannotRefract = refractionRatio * sinTheta > 1.0f;
	Vector3D direction;

	if(cannotRefract)
	{
		direction = Vector3D::reflect(unitDirection, rec.normal);
	}
	else
	{
		direction = Vector3D::refract(unitDirection, rec.normal, refractionRatio);
	}
	scattered = Ray(rec.p, direction);
	return true;
}

