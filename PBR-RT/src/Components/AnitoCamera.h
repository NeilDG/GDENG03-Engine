#pragma once
// Camera component - handles view and projection matrices

#include "../GameObjects/AnitoComponent.h"
#include "../Math/AnitoVector3D.h"
#include "../Math/AnitoMatrix4x4.h"

namespace Anito {

/**
 * AnitoCamera - Camera component
 * Supports perspective and orthographic projection
 */
class AnitoCamera : public AnitoComponent {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    AnitoCamera(const std::string& name = "Camera");
    ~AnitoCamera() override;

    void update(float deltaTime) override;

    // View matrix
    void setLookAt(const AnitoVector3D& eye, const AnitoVector3D& center, const AnitoVector3D& up);
    AnitoMatrix4x4 getViewMatrix() const { return m_viewMatrix; }

    // Projection
    void setPerspective(float fovY, float aspect, float near, float far);
    void setOrthographic(float left, float right, float bottom, float top, float near, float far);
    AnitoMatrix4x4 getProjectionMatrix() const { return m_projectionMatrix; }

    // Properties
    void setFieldOfView(float fov) { m_fov = fov; updateProjection(); }
    float getFieldOfView() const { return m_fov; }

    void setAspectRatio(float aspect) { m_aspectRatio = aspect; updateProjection(); }
    float getAspectRatio() const { return m_aspectRatio; }

    void setNearPlane(float near) { m_nearPlane = near; updateProjection(); }
    float getNearPlane() const { return m_nearPlane; }

    void setFarPlane(float far) { m_farPlane = far; updateProjection(); }
    float getFarPlane() const { return m_farPlane; }

    ProjectionType getProjectionType() const { return m_projectionType; }

private:
    void updateProjection();

    ProjectionType m_projectionType;
    AnitoMatrix4x4 m_viewMatrix;
    AnitoMatrix4x4 m_projectionMatrix;

    // Perspective parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    // Orthographic parameters
    float m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop;
};

} // namespace Anito
