#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Anito {

class AnitoVector3D {
public:
    AnitoVector3D() : m_vec(0.0f, 0.0f, 0.0f) {}
    AnitoVector3D(float x, float y, float z) : m_vec(x, y, z) {}
    AnitoVector3D(const glm::vec3& vec) : m_vec(vec) {}

    // Accessors
    float x() const { return m_vec.x; }
    float y() const { return m_vec.y; }
    float z() const { return m_vec.z; }

    void setX(float x) { m_vec.x = x; }
    void setY(float y) { m_vec.y = y; }
    void setZ(float z) { m_vec.z = z; }

    // Operators
    AnitoVector3D operator+(const AnitoVector3D& other) const {
        return AnitoVector3D(m_vec + other.m_vec);
    }

    AnitoVector3D operator-(const AnitoVector3D& other) const {
        return AnitoVector3D(m_vec - other.m_vec);
    }

    AnitoVector3D operator*(float scalar) const {
        return AnitoVector3D(m_vec * scalar);
    }

    AnitoVector3D operator/(float scalar) const {
        return AnitoVector3D(m_vec / scalar);
    }

    // Utility functions
    float length() const { return glm::length(m_vec); }
    AnitoVector3D normalized() const { return AnitoVector3D(glm::normalize(m_vec)); }

    static float dot(const AnitoVector3D& a, const AnitoVector3D& b) {
        return glm::dot(a.m_vec, b.m_vec);
    }

    static AnitoVector3D cross(const AnitoVector3D& a, const AnitoVector3D& b) {
        return AnitoVector3D(glm::cross(a.m_vec, b.m_vec));
    }

    // GLM interop
    const glm::vec3& toGLM() const { return m_vec; }
    glm::vec3& toGLM() { return m_vec; }

    // Static presets
    static AnitoVector3D zero() { return AnitoVector3D(0.0f, 0.0f, 0.0f); }
    static AnitoVector3D one() { return AnitoVector3D(1.0f, 1.0f, 1.0f); }
    static AnitoVector3D up() { return AnitoVector3D(0.0f, 1.0f, 0.0f); }
    static AnitoVector3D forward() { return AnitoVector3D(0.0f, 0.0f, 1.0f); }
    static AnitoVector3D right() { return AnitoVector3D(1.0f, 0.0f, 0.0f); }

private:
    glm::vec3 m_vec;
};

} // namespace Anito
