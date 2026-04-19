#pragma once
// Deferred rendering G-Buffer management

#include <bgfx/bgfx.h>
#include <cstdint>

namespace Anito {

/**
 * AnitoGBuffer - Geometry Buffer for deferred rendering
 * Layout:
 * - RT0: RGB = Albedo, A = Metallic
 * - RT1: RGB = Normal (world space), A = Roughness
 * - RT2: RGB = Position (world space), A = AO
 * - RT3: RGBA = Emission
 * - Depth/Stencil buffer
 */
class AnitoGBuffer {
public:
    AnitoGBuffer(uint32_t width, uint32_t height);
    ~AnitoGBuffer();

    void bind();
    void resize(uint32_t width, uint32_t height);

    bgfx::FrameBufferHandle getFrameBuffer() const { return m_frameBuffer; }

private:
    void create(uint32_t width, uint32_t height);
    void destroy();

    bgfx::FrameBufferHandle m_frameBuffer;
    uint32_t m_width;
    uint32_t m_height;

    // TODO: Add individual texture handles for sampling
};

} // namespace Anito
