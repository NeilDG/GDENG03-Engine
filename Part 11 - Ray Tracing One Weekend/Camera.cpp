#include "Camera.h"
#include <iostream>
#include "MathUtils.h"

Camera::Camera(Point3D lookFrom, Point3D lookAt, Vector3D up, float vFov, float aspectRatio, float aperture, float focusDist)
{
    const float theta = MathUtils::degreesToRadians(vFov);
    const float h = tan(theta / 2.0f);
    float viewportHeight = 2.0f * h;
    float viewportWidth = aspectRatio * viewportHeight;

    this->w = Vector3D::unitVector(lookFrom - lookAt);
    this->u = Vector3D::unitVector(Vector3D::cross(up, w));
    this->v = Vector3D::cross(w, u);

    this->origin = lookFrom;
    this->horizontal = u * viewportWidth * focusDist;
    this->vertical = v * viewportHeight * focusDist;
    this->lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - w * focusDist;
    this->lensRadius = aperture / 2.0f;
}

Ray Camera::getRay(float s, float t) const
{
    // std::cout << "Horizontal: " << this->horizontal.getX() << " Vertical: " << this->vertical.getY() << std::endl;
    // std::cout << "Lower left: " << this->lowerLeftCorner.getX() << " " << this->lowerLeftCorner.getY() << " " << this->lowerLeftCorner.getZ() << std::endl;

    Vector3D rd = Vector3D::randomInUnitDisk() * this->lensRadius;
    Vector3D offset = this->u * rd.getX() + this->v * rd.getY();
    return Ray(this->origin + offset, this->lowerLeftCorner + this->horizontal * s + this->vertical * t - origin - offset);
}

Point3D Camera::getLowerLeft() const
{
    return this->lowerLeftCorner;
}
