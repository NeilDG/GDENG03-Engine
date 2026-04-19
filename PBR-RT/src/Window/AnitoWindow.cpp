#include "AnitoWindow.h"
#include <GLFW/glfw3.h>
#include <iostream>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

namespace Anito {

AnitoWindow* AnitoWindow::create(const std::string& title, uint32_t width, uint32_t height) {
    AnitoWindow* window = new AnitoWindow(title, width, height);
    if (!window->init()) {
        delete window;
        return nullptr;
    }
    return window;
}

AnitoWindow::AnitoWindow(const std::string& title, uint32_t width, uint32_t height)
    : m_window(nullptr)
    , m_title(title)
    , m_width(width)
    , m_height(height)
    , m_resizeCallback(nullptr)
{
}

AnitoWindow::~AnitoWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool AnitoWindow::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return false;
    }

    // Don't create OpenGL context (bgfx will handle rendering)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    std::cout << "Anito Window created: " << m_title << " (" << m_width << "x" << m_height << ")" << std::endl;
    return true;
}

bool AnitoWindow::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void AnitoWindow::pollEvents() {
    glfwPollEvents();
}

void* AnitoWindow::getNativeWindowHandle() const {
#if defined(_WIN32)
    return glfwGetWin32Window(m_window);
#else
    return nullptr; // TODO: Add other platforms
#endif
}

void AnitoWindow::setResizeCallback(void (*callback)(uint32_t, uint32_t)) {
    m_resizeCallback = callback;
}

void AnitoWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    AnitoWindow* anitoWindow = static_cast<AnitoWindow*>(glfwGetWindowUserPointer(window));
    if (anitoWindow) {
        anitoWindow->m_width = width;
        anitoWindow->m_height = height;
        if (anitoWindow->m_resizeCallback) {
            anitoWindow->m_resizeCallback(width, height);
        }
    }
}

} // namespace Anito
