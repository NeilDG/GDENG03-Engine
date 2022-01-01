#pragma once
#include "Vector3D.h"
#include "Ray.h"

class Camera
{
public:
	Camera(Point3D lookFrom, Point3D lookAt, Vector3D up, float vFov, float aspectRatio, float aperture, float focusDist);
	Ray getRay(float s, float t) const;
	Point3D getLowerLeft() const;

private:
	Point3D origin;
	Point3D lowerLeftCorner;
	Vector3D horizontal;
	Vector3D vertical;
	Vector3D u, v, w;
	float lensRadius;
	
};

