#include "Camera.h"
#include <iostream>
Camera::Camera()
{
	const float aspectRatio = 16.0f / 9.0f;
    const float viewportHeight = 2.0f;
    const float viewportWidth = aspectRatio * viewportHeight;
    const float focalLength = 1.0f;

    this->origin = Point3D(0.0f, 0.0f, 0.0f);
	this->horizontal= Vector3D(viewportWidth, 0.0f, 0.0f);
    this->vertical = Vector3D(0.0f, viewportHeight, 0.0f);
    this->lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - Vector3D(0.0f, 0.0f, focalLength);
}

Ray Camera::getRay(float u, float v) const
{
    // std::cout << "Horizontal: " << this->horizontal.getX() << " Vertical: " << this->vertical.getY() << std::endl;
    // std::cout << "Lower left: " << this->lowerLeftCorner.getX() << " " << this->lowerLeftCorner.getY() << " " << this->lowerLeftCorner.getZ() << std::endl;
	
    return Ray(this->origin, this->lowerLeftCorner + this->horizontal * u + this->vertical * v - origin);
}

Point3D Camera::getLowerLeft() const
{
    return this->lowerLeftCorner;
}
