#pragma once
// FPS Camera Control component - handles first-person camera movement and look controls

#include "../GameObjects/AnitoComponent.h"
#include "../Math/AnitoVector3D.h"

namespace Anito {

class AnitoCamera;

/**
 * AnitoFPSCameraControl - First-person camera controller
 * Provides WASD movement, mouse look, and Space/Ctrl for vertical movement
 * 
 * Controls:
 * - WASD: Forward/Left/Backward/Right movement
 * - Left Mouse Button + Drag: Look around (scene viewer style)
 * - Space/Left Ctrl: Move up/down
 * - Scroll Wheel: Adjust movement speed
 * - Left Shift: Sprint (2x speed multiplier)
 */
class AnitoFPSCameraControl : public AnitoComponent {
public:
    AnitoFPSCameraControl(const std::string& name = "FPSCameraControl");
    ~AnitoFPSCameraControl() override;

    void update(float deltaTime) override;

    // Movement settings
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }
    float getMovementSpeed() const { return m_movementSpeed; }

    void setSprintMultiplier(float multiplier) { m_sprintMultiplier = multiplier; }
    float getSprintMultiplier() const { return m_sprintMultiplier; }

    // Look settings
    void setLookSensitivity(float sensitivity) { m_lookSensitivity = sensitivity; }
    float getLookSensitivity() const { return m_lookSensitivity; }

    void setInvertY(bool invert) { m_invertY = invert; }
    bool getInvertY() const { return m_invertY; }

    // Camera access
    AnitoCamera* getCamera() const { return m_camera; }

    // Get current orientation for debugging
    float getYaw() const { return m_yaw; }
    float getPitch() const { return m_pitch; }

    // Set orientation (useful for initialization)
    void setYaw(float yaw) { m_yaw = yaw; }
    void setPitch(float pitch) { 
        // Constrain pitch to prevent gimbal lock
        const float maxPitch = 89.0f;
        m_pitch = pitch;
        if (m_pitch > maxPitch) m_pitch = maxPitch;
        if (m_pitch < -maxPitch) m_pitch = -maxPitch;
    }

    // Enable/disable controls
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    void handleMovementInput(float deltaTime);
    void handleLookInput(float deltaTime);
    void updateCameraVectors();

    AnitoCamera* m_camera;

    // Movement parameters
    float m_movementSpeed;
    float m_sprintMultiplier;

    // Look parameters
    float m_lookSensitivity;
    bool m_invertY;
    float m_yaw;   // Rotation around Y axis (left/right)
    float m_pitch; // Rotation around X axis (up/down)

    // Camera vectors
    AnitoVector3D m_forward;
    AnitoVector3D m_right;
    AnitoVector3D m_up;

    // Control state
    bool m_enabled;
    bool m_firstMouseMove;
};

} // namespace Anito
