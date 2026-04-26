#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <memory>
#include <string>

namespace Anito {

// Forward declarations
class AnitoShader;

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
    void bindIBLTextures(std::shared_ptr<AnitoShader> shader); // Bind IBL textures for current draw call
    bgfx::TextureHandle getEnvCubemap() const { return m_envCubemap; }
    bgfx::TextureHandle getIrradianceMap() const { return m_irradianceMap; }
    bgfx::TextureHandle getPrefilterMap() const { return m_prefilterMap; }
    bool isIBLReady() const { return bgfx::isValid(m_envCubemap) && bgfx::isValid(m_irradianceMap) && bgfx::isValid(m_prefilterMap); }
    void setEnableIBL(bool enable) { m_enableIBL = enable; }
    bool getEnableIBL() const { return m_enableIBL; }

private:
    AnitoRenderer();
    ~AnitoRenderer();
    AnitoRenderer(const AnitoRenderer&) = delete;
    AnitoRenderer& operator=(const AnitoRenderer&) = delete;

    void init(void* nativeWindowHandle, uint32_t width, uint32_t height);
    void release();

    // IBL prefiltering helpers
    void generateIrradianceMap(bgfx::TextureHandle envCubemap);
    void generatePrefilterMap(bgfx::TextureHandle envCubemap);

    static AnitoRenderer* s_instance;

    uint32_t m_width;
    uint32_t m_height;
    bgfx::ViewId m_mainViewId;
    uint32_t m_resetFlags;

    // Skybox/IBL
    bgfx::TextureHandle m_envCubemap = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle m_irradianceMap = BGFX_INVALID_HANDLE;      // Diffuse IBL (32x32 cubemap)
    bgfx::TextureHandle m_prefilterMap = BGFX_INVALID_HANDLE;       // Specular IBL (512x512 with mips)
    bgfx::ProgramHandle m_skyboxProgram = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_skyboxCubeUniform = BGFX_INVALID_HANDLE;

    // Persistent skybox uniforms (don't create/destroy every frame!)
    bgfx::UniformHandle m_viewProjInvUniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_cameraPosUniform = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_skyboxParamsUniform = BGFX_INVALID_HANDLE;

    bool m_enableIBL = true;
};

} // namespace Anito
