#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <memory>
#include <string>

namespace Anito {

/**
 * AnitoRenderer - Core rendering abstraction over bgfx
 * Implements deferred rendering pipeline with PBR materials
 * Following Anito Engine architecture standards
 */
class AnitoRenderer {
public:
    static AnitoRenderer* getInstance();
    static void initialize(void* nativeWindowHandle, uint32_t width, uint32_t height);
    static void destroy();

    // Core rendering functions
    void beginFrame();
    void endFrame();
    void resize(uint32_t width, uint32_t height);

    // Getters
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    bgfx::ViewId getMainViewId() const { return m_mainViewId; }

    // Shader compilation helpers
    bgfx::ShaderHandle createShader(const std::string& shaderPath);
    bgfx::ProgramHandle createProgram(const std::string& vsPath, const std::string& fsPath);

private:
    AnitoRenderer();
    ~AnitoRenderer();
    AnitoRenderer(const AnitoRenderer&) = delete;
    AnitoRenderer& operator=(const AnitoRenderer&) = delete;

    void init(void* nativeWindowHandle, uint32_t width, uint32_t height);
    void release();

    static AnitoRenderer* s_instance;

    uint32_t m_width;
    uint32_t m_height;
    bgfx::ViewId m_mainViewId;
    uint32_t m_resetFlags;
};

} // namespace Anito
