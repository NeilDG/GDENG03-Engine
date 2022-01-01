#include "Sphere.h"
#include <cmath>

Sphere::Sphere()
{
}

Sphere::Sphere(Point3D center, float radius, std::shared_ptr<AMaterial> material)
{
	this->center = center;
	this->radius = radius;
    this->refMaterial = material;
}

bool Sphere::hit(const Ray r, float tMin, float tMax, HitRecord& rec) const
{
    Vector3D oc = r.getOrigin() - center;
    float a = r.getDirection().lengthSquared();
    float halfB = Vector3D::dot(oc, r.getDirection());
    float c = oc.lengthSquared() - radius * radius;

    auto discriminant = halfB * halfB - a * c;
    if (discriminant < 0.0f)
    {
        return false;
    }

    float sqrtd = sqrt(discriminant);

    float root = (-halfB - sqrtd) / a;
	if(root < tMin || tMax < root)
	{
        root = (-halfB + sqrtd) / a;
		if(root < tMin || tMax < root)
		{
            return false;
		}
	}

    rec.t = root;
    rec.p = r.at(rec.t);
    Vector3D outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);
    rec.refMaterial = refMaterial;

    return true;
}

Point3D Sphere::getCenter()
{
    return this->center;
}

float Sphere::getRadius()
{
    return this->radius;
}
