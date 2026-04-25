#pragma once
// Input management system

#include "../Math/AnitoVector2D.h"
#include <GLFW/glfw3.h>
#include <unordered_map>

namespace Anito {

/**
 * AnitoInputManager - Handles keyboard and mouse input
 * Singleton pattern following reference implementation
 */
class AnitoInputManager {
public:
    enum class MouseButton {
        Left = GLFW_MOUSE_BUTTON_LEFT,
        Right = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE
    };

    static AnitoInputManager* getInstance();
    static void initialize(GLFWwindow* window);
    static void destroy();

    void update();

    // Keyboard
    bool isKeyDown(int key) const;
    bool isKeyPressed(int key) const;
    bool isKeyReleased(int key) const;

    // Mouse
    bool isMouseButtonDown(MouseButton button) const;
    bool isMouseButtonPressed(MouseButton button) const;
    bool isMouseButtonReleased(MouseButton button) const;

    AnitoVector2D getMousePosition() const { return m_mousePosition; }
    AnitoVector2D getMouseDelta() const { return m_mouseDelta; }
    float getMouseScrollDelta() const { return m_scrollDelta; }

private:
    AnitoInputManager();
    ~AnitoInputManager();
    AnitoInputManager(const AnitoInputManager&) = delete;
    AnitoInputManager& operator=(const AnitoInputManager&) = delete;

    static AnitoInputManager* s_instance;

    GLFWwindow* m_window;
    AnitoVector2D m_mousePosition;
    AnitoVector2D m_lastMousePosition;
    AnitoVector2D m_mouseDelta;
    float m_scrollDelta;

    // State tracking for pressed/released detection
    std::unordered_map<int, bool> m_previousKeyStates;
    std::unordered_map<int, bool> m_previousMouseStates;

    // Special key states for toggles
    bool m_wasZPressed = false;
    bool m_wasSpacePressed = false;

    // Callbacks
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

} // namespace Anito
