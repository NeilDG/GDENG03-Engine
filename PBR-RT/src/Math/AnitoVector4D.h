#pragma once
#include <glm/glm.hpp>

namespace Anito {

class AnitoVector4D {
public:
    AnitoVector4D() : m_vec(0.0f, 0.0f, 0.0f, 0.0f) {}
    AnitoVector4D(float x, float y, float z, float w) : m_vec(x, y, z, w) {}
    AnitoVector4D(const glm::vec4& vec) : m_vec(vec) {}

    float x() const { return m_vec.x; }
    float y() const { return m_vec.y; }
    float z() const { return m_vec.z; }
    float w() const { return m_vec.w; }

    const glm::vec4& toGLM() const { return m_vec; }
    glm::vec4& toGLM() { return m_vec; }

private:
    glm::vec4 m_vec;
};

} // namespace Anito
