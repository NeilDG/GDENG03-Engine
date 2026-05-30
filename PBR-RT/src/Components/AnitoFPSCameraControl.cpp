#include "AnitoFPSCameraControl.h"
#include "AnitoCamera.h"
#include "../GameObjects/AnitoGameObject.h"
#include "../Input/AnitoInputManager.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <iostream>

namespace Anito {

AnitoFPSCameraControl::AnitoFPSCameraControl(const std::string& name)
    : AnitoComponent(name, ComponentType::Script)
    , m_camera(nullptr)
    , m_movementSpeed(5.0f)
    , m_sprintMultiplier(2.0f)
    , m_lookSensitivity(0.1f)
    , m_invertY(false)
    , m_yaw(0.0f)
    , m_pitch(0.0f)
    , m_forward(AnitoVector3D::forward())
    , m_right(AnitoVector3D::right())
    , m_up(AnitoVector3D::up())
    , m_enabled(true)
    , m_firstMouseMove(true)
{
}

AnitoFPSCameraControl::~AnitoFPSCameraControl() {
}

void AnitoFPSCameraControl::update(float deltaTime) {
    if (!m_enabled) return;

    // Find camera component on first update
    if (!m_camera && m_owner) {
        m_camera = m_owner->getComponent<AnitoCamera>();
        if (m_camera) {
            std::cout << "[FPSCameraControl] Camera component found and attached" << std::endl;
        }
    }

    if (!m_camera) return;

    // Handle input
    handleLookInput(deltaTime);
    handleMovementInput(deltaTime);

    // Update camera vectors based on current orientation
    updateCameraVectors();

    // Update camera's view matrix
    if (m_owner) {
        AnitoVector3D position = m_owner->getPosition();
        AnitoVector3D center = position + m_forward;
        m_camera->setLookAt(position, center, m_up);
    }
}

void AnitoFPSCameraControl::handleLookInput(float deltaTime) {
    auto* input = AnitoInputManager::getInstance();
    if (!input) return;

    // Rotate camera only when left mouse button is held (scene viewer style)
    bool shouldLook = input->isMouseButtonDown(AnitoInputManager::MouseButton::Left);

    if (shouldLook) {
        // Reset first mouse flag when button is first pressed to prevent camera jump
        if (m_firstMouseMove) {
            m_firstMouseMove = false;
            return; // Skip this frame to establish baseline position
        }

        AnitoVector2D mouseDelta = input->getMouseDelta();

        // Apply sensitivity
        float xOffset = mouseDelta.x() * m_lookSensitivity;
        float yOffset = mouseDelta.y() * m_lookSensitivity;

        // Invert Y if needed
        if (m_invertY) {
            yOffset = -yOffset;
        } else {
            // Default is to invert (mouse down = look up)
            yOffset = -yOffset;
        }

        // Update yaw and pitch
        m_yaw += xOffset;
        m_pitch += yOffset;

        // Constrain pitch to prevent gimbal lock
        const float maxPitch = 89.0f;
        if (m_pitch > maxPitch) {
            m_pitch = maxPitch;
        }
        if (m_pitch < -maxPitch) {
            m_pitch = -maxPitch;
        }

        // Wrap yaw to [0, 360)
        while (m_yaw >= 360.0f) m_yaw -= 360.0f;
        while (m_yaw < 0.0f) m_yaw += 360.0f;
    } else {
        // Reset first mouse flag when button is released
        m_firstMouseMove = true;
    }

    // Handle mouse scroll for speed adjustment
    float scrollDelta = input->getMouseScrollDelta();
    if (scrollDelta != 0.0f) {
        m_movementSpeed += scrollDelta * 0.5f;
        if (m_movementSpeed < 0.5f) m_movementSpeed = 0.5f;
        if (m_movementSpeed > 50.0f) m_movementSpeed = 50.0f;

        std::cout << "[FPSCameraControl] Movement speed: " << m_movementSpeed << " units/sec" << std::endl;
    }
}

void AnitoFPSCameraControl::handleMovementInput(float deltaTime) {
    auto* input = AnitoInputManager::getInstance();
    if (!input || !m_owner) return;

    // Calculate effective speed (with sprint multiplier)
    float speed = m_movementSpeed;
    if (input->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        speed *= m_sprintMultiplier;
    }

    float velocity = speed * deltaTime;
    AnitoVector3D movement = AnitoVector3D::zero();

    // Forward/Backward (W/S)
    if (input->isKeyDown(GLFW_KEY_W)) {
        movement = movement + (m_forward * velocity);
    }
    if (input->isKeyDown(GLFW_KEY_S)) {
        movement = movement - (m_forward * velocity);
    }

    // Left/Right (A/D)
    if (input->isKeyDown(GLFW_KEY_A)) {
        movement = movement - (m_right * velocity);
    }
    if (input->isKeyDown(GLFW_KEY_D)) {
        movement = movement + (m_right * velocity);
    }

    // Up/Down (E/Q or Space/Left Ctrl)
    if (input->isKeyDown(GLFW_KEY_E)) {
        movement = movement + (AnitoVector3D::up() * velocity);
    }
    if (input->isKeyDown(GLFW_KEY_Q)) {
        movement = movement - (AnitoVector3D::up() * velocity);
    }

    // Apply movement
    if (movement.length() > 0.001f) {
        AnitoVector3D newPosition = m_owner->getPosition() + movement;
        m_owner->setPosition(newPosition);
    }
}

void AnitoFPSCameraControl::updateCameraVectors() {
    // Convert angles to radians
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    // Calculate forward vector from yaw and pitch
    glm::vec3 forward;
    forward.x = cos(pitchRad) * sin(yawRad);
    forward.y = sin(pitchRad);
    forward.z = cos(pitchRad) * cos(yawRad);

    m_forward = AnitoVector3D(glm::normalize(forward));

    // Calculate right vector (perpendicular to forward and world up)
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    m_right = AnitoVector3D(right);

    // Calculate up vector (perpendicular to forward and right)
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    m_up = AnitoVector3D(up);
}

} // namespace Anito
