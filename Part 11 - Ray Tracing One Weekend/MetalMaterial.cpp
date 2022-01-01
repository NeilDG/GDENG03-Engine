#include "MetalMaterial.h"

MetalMaterial::MetalMaterial(const Color a, float f)
{
	this->albedo = a;
	if(f < 1.0f)
	{
		this->fuzziness = f;
	}
	else
	{
		this->fuzziness = 1.0f;
	}
}

bool MetalMaterial::scatter(const Ray rIn, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	Vector3D reflected = Vector3D::reflect(Vector3D::unitVector(rIn.getDirection()), rec.normal);
	scattered = Ray(rec.p, reflected + Vector3D::randomInUnitSphere() * this->fuzziness);
	attenuation = albedo;

	return (Vector3D::dot(scattered.getDirection(), rec.normal) > 0.0f);
	
}
