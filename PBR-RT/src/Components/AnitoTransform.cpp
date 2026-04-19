#include "AnitoTransform.h"
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Anito {

AnitoTransform::AnitoTransform(const std::string& name)
    : AnitoComponent(name, ComponentType::Transform)
    , m_position(AnitoVector3D::zero())
    , m_rotation(AnitoVector3D::zero())
    , m_scale(AnitoVector3D::one())
{
    updateMatrix();
}

AnitoTransform::~AnitoTransform() {
}

void AnitoTransform::setPosition(const AnitoVector3D& pos) {
    m_position = pos;
    updateMatrix();
}

void AnitoTransform::setRotation(const AnitoVector3D& rot) {
    m_rotation = rot;
    updateMatrix();
}

void AnitoTransform::setScale(const AnitoVector3D& scale) {
    m_scale = scale;
    updateMatrix();
}

void AnitoTransform::updateMatrix() {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), m_position.toGLM());
    glm::mat4 r = glm::eulerAngleXYZ(m_rotation.x(), m_rotation.y(), m_rotation.z());
    glm::mat4 s = glm::scale(glm::mat4(1.0f), m_scale.toGLM());
    m_matrix = AnitoMatrix4x4(t * r * s);
}

} // namespace Anito
