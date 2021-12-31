#pragma once
#include "Vector3D.h"
#include "Ray.h"

class Camera
{
public:
	Camera();
	Ray getRay(float u, float v) const;
	Point3D getLowerLeft() const;

private:
	Point3D origin;
	Point3D lowerLeftCorner;
	Vector3D horizontal;
	Vector3D vertical;
	
};

