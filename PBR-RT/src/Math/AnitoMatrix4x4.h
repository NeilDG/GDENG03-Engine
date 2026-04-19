#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Anito {

class AnitoMatrix4x4 {
public:
    AnitoMatrix4x4() : m_mat(1.0f) {} // Identity matrix
    AnitoMatrix4x4(const glm::mat4& mat) : m_mat(mat) {}

    // Static factory methods
    static AnitoMatrix4x4 identity() { return AnitoMatrix4x4(glm::mat4(1.0f)); }

    static AnitoMatrix4x4 translation(float x, float y, float z) {
        return AnitoMatrix4x4(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
    }

    static AnitoMatrix4x4 rotationX(float angleRadians) {
        return AnitoMatrix4x4(glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(1, 0, 0)));
    }

    static AnitoMatrix4x4 rotationY(float angleRadians) {
        return AnitoMatrix4x4(glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0, 1, 0)));
    }

    static AnitoMatrix4x4 rotationZ(float angleRadians) {
        return AnitoMatrix4x4(glm::rotate(glm::mat4(1.0f), angleRadians, glm::vec3(0, 0, 1)));
    }

    static AnitoMatrix4x4 scale(float x, float y, float z) {
        return AnitoMatrix4x4(glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z)));
    }

    static AnitoMatrix4x4 perspective(float fovY, float aspect, float near, float far) {
        return AnitoMatrix4x4(glm::perspective(fovY, aspect, near, far));
    }

    static AnitoMatrix4x4 ortho(float left, float right, float bottom, float top, float near, float far) {
        return AnitoMatrix4x4(glm::ortho(left, right, bottom, top, near, far));
    }

    static AnitoMatrix4x4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
        return AnitoMatrix4x4(glm::lookAt(eye, center, up));
    }

    // Operators
    AnitoMatrix4x4 operator*(const AnitoMatrix4x4& other) const {
        return AnitoMatrix4x4(m_mat * other.m_mat);
    }

    // Access raw data
    const float* data() const { return glm::value_ptr(m_mat); }
    float* data() { return glm::value_ptr(m_mat); }

    // GLM interop
    const glm::mat4& toGLM() const { return m_mat; }
    glm::mat4& toGLM() { return m_mat; }

    void setMatrix(const float* matrix16) {
        memcpy(glm::value_ptr(m_mat), matrix16, sizeof(float) * 16);
    }

private:
    glm::mat4 m_mat;
};

} // namespace Anito
