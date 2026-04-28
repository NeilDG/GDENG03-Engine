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

private:
    std::unique_ptr<AnitoGBuffer> m_gBuffer;
    uint32_t m_width;
    uint32_t m_height;

    // View IDs for different passes
    bgfx::ViewId m_geometryViewId;  // View 2: Geometry pass (writes to G-Buffer)
    bgfx::ViewId m_lightingViewId;  // View 1: Lighting pass (reads G-Buffer, outputs final image)
};

} // namespace Anito
