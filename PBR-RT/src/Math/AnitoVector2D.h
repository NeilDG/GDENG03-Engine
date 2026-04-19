#pragma once
#include <glm/glm.hpp>

namespace Anito {

class AnitoVector2D {
public:
    AnitoVector2D() : m_vec(0.0f, 0.0f) {}
    AnitoVector2D(float x, float y) : m_vec(x, y) {}
    AnitoVector2D(const glm::vec2& vec) : m_vec(vec) {}

    float x() const { return m_vec.x; }
    float y() const { return m_vec.y; }

    void setX(float x) { m_vec.x = x; }
    void setY(float y) { m_vec.y = y; }

    const glm::vec2& toGLM() const { return m_vec; }
    glm::vec2& toGLM() { return m_vec; }

private:
    glm::vec2 m_vec;
};

} // namespace Anito
