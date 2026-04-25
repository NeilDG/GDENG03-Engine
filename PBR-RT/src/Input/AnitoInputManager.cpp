#include "AnitoInputManager.h"
#include "../Renderer/AnitoRenderer.h"
#include "../Renderer/AnitoPBRTestScenes.h"

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

    // Handle IBL toggle (Z key)
    bool zDown = isKeyDown(GLFW_KEY_Z);
    if (zDown && !m_wasZPressed) {
        auto* renderer = AnitoRenderer::getInstance();
        if (renderer) {
            renderer->setEnableIBL(!renderer->getEnableIBL());
            std::cout << "[Input] IBL " << (renderer->getEnableIBL() ? "enabled" : "disabled") << std::endl;
        }
    }
    m_wasZPressed = zDown;

    // Handle scene switch (Space key)
    bool spaceDown = isKeyDown(GLFW_KEY_SPACE);
    if (spaceDown && !m_wasSpacePressed) {
        auto* testScenes = AnitoPBRTestScenes::getCurrentInstance();
        if (testScenes) {
            testScenes->switchToNextScene();
        } else {
            std::cout << "[Input] Space pressed - but no test scenes instance available" << std::endl;
        }
    }
    m_wasSpacePressed = spaceDown;

    // Note: Previous states are stored in isKeyPressed/isKeyReleased methods
    // This approach ensures we capture state at the moment of query
}

bool AnitoInputManager::isKeyDown(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool AnitoInputManager::isKeyPressed(int key) const {
    if (!m_window) return false;

    bool currentlyDown = (glfwGetKey(m_window, key) == GLFW_PRESS);
    bool wasDown = false;

    auto it = m_previousKeyStates.find(key);
    if (it != m_previousKeyStates.end()) {
        wasDown = it->second;
    }

    // Update previous state for next frame
    const_cast<AnitoInputManager*>(this)->m_previousKeyStates[key] = currentlyDown;

    // Pressed = transition from not-down to down
    return currentlyDown && !wasDown;
}

bool AnitoInputManager::isKeyReleased(int key) const {
    if (!m_window) return false;

    bool currentlyDown = (glfwGetKey(m_window, key) == GLFW_PRESS);
    bool wasDown = false;

    auto it = m_previousKeyStates.find(key);
    if (it != m_previousKeyStates.end()) {
        wasDown = it->second;
    }

    // Released = transition from down to not-down
    return !currentlyDown && wasDown;
}

bool AnitoInputManager::isMouseButtonDown(MouseButton button) const {
    if (!m_window) return false;
    return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS;
}

bool AnitoInputManager::isMouseButtonPressed(MouseButton button) const {
    if (!m_window) return false;

    int buttonInt = static_cast<int>(button);
    bool currentlyDown = (glfwGetMouseButton(m_window, buttonInt) == GLFW_PRESS);
    bool wasDown = false;

    auto it = m_previousMouseStates.find(buttonInt);
    if (it != m_previousMouseStates.end()) {
        wasDown = it->second;
    }

    // Update previous state for next frame
    const_cast<AnitoInputManager*>(this)->m_previousMouseStates[buttonInt] = currentlyDown;

    // Pressed = transition from not-down to down
    return currentlyDown && !wasDown;
}

bool AnitoInputManager::isMouseButtonReleased(MouseButton button) const {
    if (!m_window) return false;

    int buttonInt = static_cast<int>(button);
    bool currentlyDown = (glfwGetMouseButton(m_window, buttonInt) == GLFW_PRESS);
    bool wasDown = false;

    auto it = m_previousMouseStates.find(buttonInt);
    if (it != m_previousMouseStates.end()) {
        wasDown = it->second;
    }

    // Released = transition from down to not-down
    return !currentlyDown && wasDown;
}

void AnitoInputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (s_instance) {
        s_instance->m_scrollDelta = static_cast<float>(yoffset);
    }
}

} // namespace Anito
