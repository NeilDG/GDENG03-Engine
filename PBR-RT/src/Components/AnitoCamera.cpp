#include "AnitoCamera.h"
#include "../GameObjects/AnitoGameObject.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Anito {

AnitoCamera::AnitoCamera(const std::string& name)
    : AnitoComponent(name, ComponentType::Camera)
    , m_projectionType(ProjectionType::Perspective)
    , m_fov(glm::radians(60.0f))
    , m_aspectRatio(16.0f / 9.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(1000.0f)
    , m_orthoLeft(-10.0f)
    , m_orthoRight(10.0f)
    , m_orthoBottom(-10.0f)
    , m_orthoTop(10.0f)
{
    updateProjection();
}

AnitoCamera::~AnitoCamera() {
}

void AnitoCamera::update(float deltaTime) {
    // Update view matrix from owner's position
    if (m_owner) {
        AnitoVector3D pos = m_owner->getPosition();
        AnitoVector3D rot = m_owner->getRotation();

        // Calculate forward and up vectors from rotation
        glm::vec3 forward(
            cos(rot.y()) * sin(rot.x()),
            sin(rot.y()),
            cos(rot.y()) * cos(rot.x())
        );

        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 center = pos.toGLM() + forward;

        m_viewMatrix = AnitoMatrix4x4::lookAt(pos.toGLM(), center, up);
    }
}

void AnitoCamera::setLookAt(const AnitoVector3D& eye, const AnitoVector3D& center, const AnitoVector3D& up) {
    m_viewMatrix = AnitoMatrix4x4::lookAt(eye.toGLM(), center.toGLM(), up.toGLM());
}

void AnitoCamera::setPerspective(float fovY, float aspect, float near, float far) {
    m_projectionType = ProjectionType::Perspective;
    m_fov = fovY;
    m_aspectRatio = aspect;
    m_nearPlane = near;
    m_farPlane = far;
    updateProjection();
}

void AnitoCamera::setOrthographic(float left, float right, float bottom, float top, float near, float far) {
    m_projectionType = ProjectionType::Orthographic;
    m_orthoLeft = left;
    m_orthoRight = right;
    m_orthoBottom = bottom;
    m_orthoTop = top;
    m_nearPlane = near;
    m_farPlane = far;
    updateProjection();
}

void AnitoCamera::updateProjection() {
    if (m_projectionType == ProjectionType::Perspective) {
        m_projectionMatrix = AnitoMatrix4x4::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    } else {
        m_projectionMatrix = AnitoMatrix4x4::ortho(m_orthoLeft, m_orthoRight, m_orthoBottom, m_orthoTop, m_nearPlane, m_farPlane);
    }
}

} // namespace Anito
