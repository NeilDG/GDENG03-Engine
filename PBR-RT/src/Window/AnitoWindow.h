#pragma once
#include <string>

struct GLFWwindow;

namespace Anito {

/**
 * AnitoWindow - Window management using GLFW
 */
class AnitoWindow {
public:
    static AnitoWindow* create(const std::string& title, uint32_t width, uint32_t height);
    ~AnitoWindow();

    bool shouldClose() const;
    void pollEvents();
    void* getNativeWindowHandle() const;

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    GLFWwindow* getGLFWWindow() const { return m_window; }

    void setResizeCallback(void (*callback)(uint32_t, uint32_t));

private:
    AnitoWindow(const std::string& title, uint32_t width, uint32_t height);
    bool init();

    GLFWwindow* m_window;
    std::string m_title;
    uint32_t m_width;
    uint32_t m_height;
    void (*m_resizeCallback)(uint32_t, uint32_t);

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

} // namespace Anito
