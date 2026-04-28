#pragma once
// Deferred Rendering Pipeline Manager

#include <bgfx/bgfx.h>
#include <memory>
#include <cstdint>

namespace Anito {

class AnitoGBuffer;

/**
 * AnitoDeferredRenderer - Manages deferred rendering pipeline
 * 
 * Rendering stages:
 * 1. Geometry Pass - Render to G-Buffer (View 2)
 * 2. Lighting Pass - Process lights using G-Buffer data (View 1)
 * 3. Forward Pass - Transparent objects, particles, etc.
 * 4. Post-Processing - Tone mapping, bloom, etc.
 * 
 * Reference: BGFX Example 21-deferred, Filament deferred pipeline
 */
class AnitoDeferredRenderer {
public:
    AnitoDeferredRenderer(uint32_t width, uint32_t height);
    ~AnitoDeferredRenderer();

    void resize(uint32_t width, uint32_t height);

    // Geometry pass (writes to G-Buffer)
    void beginGeometryPass();
    void endGeometryPass();
    bgfx::ViewId getGeometryViewId() const { return m_geometryViewId; }

    // Lighting pass (reads from G-Buffer, outputs to backbuffer)
    void beginLightingPass();
    void endLightingPass();
    bgfx::ViewId getLightingViewId() const { return m_lightingViewId; }

    // Full render pipeline
    void render();

    // Access to G-Buffer
    AnitoGBuffer* getGBuffer() const { return m_gBuffer.get(); }

    // Shader and uniform management
    void setLightingShader(bgfx::ProgramHandle shader);
    void setDebugShader(bgfx::ProgramHandle shader);  // Step 10
    void setGBufferUniforms(bgfx::UniformHandle s_gbuffer0, bgfx::UniformHandle s_gbuffer1,
                            bgfx::UniformHandle s_gbuffer2, bgfx::UniformHandle s_gbuffer3);
    void setCameraUniforms(bgfx::UniformHandle u_cameraPos);
    void setIBLUniforms(bgfx::UniformHandle s_irradianceMap, bgfx::UniformHandle s_prefilterMap, bgfx::UniformHandle s_brdfLUT);

    // Debug visualization (Step 10)
    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    bool getDebugMode() const { return m_debugMode; }
    void renderDebugVisualization();

private:
    std::unique_ptr<AnitoGBuffer> m_gBuffer;
    uint32_t m_width;
    uint32_t m_height;

    // View IDs for different passes
    bgfx::ViewId m_geometryViewId;  // View 2: Geometry pass (writes to G-Buffer)
    bgfx::ViewId m_lightingViewId;  // View 1: Lighting pass (reads G-Buffer, outputs final image)

    // Lighting pass resources
    bgfx::ProgramHandle m_lightingShader = BGFX_INVALID_HANDLE;
    bgfx::ProgramHandle m_debugShader = BGFX_INVALID_HANDLE;  // Step 10
    bool m_debugMode = false;  // Step 10: Toggle G-Buffer visualization

    // G-Buffer texture samplers
    bgfx::UniformHandle m_s_gbuffer0 = BGFX_INVALID_HANDLE;  // Albedo + Metallic
    bgfx::UniformHandle m_s_gbuffer1 = BGFX_INVALID_HANDLE;  // Normal + Roughness
    bgfx::UniformHandle m_s_gbuffer2 = BGFX_INVALID_HANDLE;  // Position + AO
    bgfx::UniformHandle m_s_gbuffer3 = BGFX_INVALID_HANDLE;  // Emission

    // IBL texture samplers
    bgfx::UniformHandle m_s_irradianceMap = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_s_prefilterMap = BGFX_INVALID_HANDLE;
    bgfx::UniformHandle m_s_brdfLUT = BGFX_INVALID_HANDLE;

    // Camera uniforms
    bgfx::UniformHandle m_u_cameraPos = BGFX_INVALID_HANDLE;
};

} // namespace Anito
