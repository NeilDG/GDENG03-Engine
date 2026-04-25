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

    // Skybox/IBL
    void loadEnvironmentMap(const std::string& hdrPath);
    void renderSkybox(const float* viewProjInv, const float* cameraPos);
    bgfx::TextureHandle getEnvCubemap() const { return m_envCubemap; }
    bool isIBLReady() const { return bgfx::isValid(m_envCubemap); }
    void setEnableIBL(bool enable) { m_enableIBL = enable; }
    bool getEnableIBL() const { return m_enableIBL; }

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

    // Skybox/IBL
    bgfx::TextureHandle m_envCubemap = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle m_skyboxProgram = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_skyboxCubeUniform = BGFX_INVALID_HANDLE;
    bool m_enableIBL = true;
};

} // namespace Anito
