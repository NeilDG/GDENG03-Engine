#pragma once
// Deferred Rendering Pipeline Manager

#include <memory>
#include <cstdint>

namespace Anito {

class AnitoGBuffer;

/**
 * AnitoDeferredRenderer - Manages deferred rendering pipeline
 * 
 * Rendering stages:
 * 1. Geometry Pass - Render to G-Buffer
 * 2. Lighting Pass - Process lights using G-Buffer data
 * 3. Forward Pass - Transparent objects, particles, etc.
 * 4. Post-Processing - Tone mapping, bloom, etc.
 */
class AnitoDeferredRenderer {
public:
    AnitoDeferredRenderer(uint32_t width, uint32_t height);
    ~AnitoDeferredRenderer();

    void resize(uint32_t width, uint32_t height);

    void beginGeometryPass();
    void endGeometryPass();

    void beginLightingPass();
    void endLightingPass();

    void render();

private:
    std::unique_ptr<AnitoGBuffer> m_gBuffer;
    uint32_t m_width;
    uint32_t m_height;
};

} // namespace Anito
