#include "AnitoInputManager.h"

namespace Anito {

AnitoInputManager* AnitoInputManager::s_instance = nullptr;

AnitoInputManager* AnitoInputManager::getInstance() {
    return s_instance;
}

void AnitoInputManager::initialize(GLFWwindow* window) {
    if (s_instance == nullptr) {
        s_instance = new AnitoInputManager();
        s_instance->m_window = window;
        glfwSetScrollCallback(window, scrollCallback);
    }
}

void AnitoInputManager::destroy() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoInputManager::AnitoInputManager()
    : m_window(nullptr)
    , m_scrollDelta(0.0f)
{
}

AnitoInputManager::~AnitoInputManager() {
}

void AnitoInputManager::update() {
    if (!m_window) return;

    // Update mouse position
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    m_lastMousePosition = m_mousePosition;
    m_mousePosition = AnitoVector2D(static_cast<float>(xpos), static_cast<float>(ypos));
    m_mouseDelta = AnitoVector2D(
        m_mousePosition.x() - m_lastMousePosition.x(),
        m_mousePosition.y() - m_lastMousePosition.y()
    );

    // Reset scroll delta (will be updated by callback)
    m_scrollDelta = 0.0f;
}

bool AnitoInputManager::isKeyDown(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool AnitoInputManager::isKeyPressed(int key) const {
    // TODO: Implement pressed/released state tracking
    return isKeyDown(key);
}

bool AnitoInputManager::isKeyReleased(int key) const {
    // TODO: Implement pressed/released state tracking
    return false;
}

bool AnitoInputManager::isMouseButtonDown(MouseButton button) const {
    if (!m_window) return false;
    return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS;
}

bool AnitoInputManager::isMouseButtonPressed(MouseButton button) const {
    // TODO: Implement pressed/released state tracking
    return isMouseButtonDown(button);
}

bool AnitoInputManager::isMouseButtonReleased(MouseButton button) const {
    // TODO: Implement pressed/released state tracking
    return false;
}

void AnitoInputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (s_instance) {
        s_instance->m_scrollDelta = static_cast<float>(yoffset);
    }
}

} // namespace Anito
