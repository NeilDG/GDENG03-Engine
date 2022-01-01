#include "DiffuseMaterial.h"

DiffuseMaterial::DiffuseMaterial(const Color a)
{
	this->albedo = a;
}

bool DiffuseMaterial::scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	Vector3D scatterDirection = rec.normal + Vector3D::randomUnitVector();

	//Catch degenerate scatter direction
	if(scatterDirection.nearZero())
	{
		scatterDirection = rec.normal;
	}

	scattered = Ray(rec.p, scatterDirection);
	attenuation = albedo;
	return true;
}
