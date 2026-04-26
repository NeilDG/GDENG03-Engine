#pragma once
// Deferred rendering G-Buffer management
// Following industry best practices: BGFX examples, Filament PBR documentation
// Mobile-friendly texture formats for mid-range to photorealistic games

#include <bgfx/bgfx.h>
#include <cstdint>

namespace Anito {

/**
 * AnitoGBuffer - Geometry Buffer for deferred rendering
 * 
 * Mobile-optimized G-Buffer Layout:
 * - RT0 (RGBA8): RGB = Albedo (sRGB), A = Metallic [0,1]
 * - RT1 (RGBA8): RGB = Normal (encoded [-1,1] -> [0,1]), A = Roughness [0,1]
 * - RT2 (RGBA16F): RGB = World Position (requires float precision), A = AO [0,1]
 * - RT3 (RGBA8): RGB = Emission (LDR-encoded HDR), A = unused
 * - Depth (D24S8): 24-bit depth + 8-bit stencil
 * 
 * Total bandwidth: 3x RGBA8 (12 bytes) + 1x RGBA16F (8 bytes) + D24S8 (4 bytes) = 24 bytes/pixel
 * At 1080p: ~50MB framebuffer memory (mobile-friendly)
 * 
 * Reference: 
 * - BGFX Example 21-deferred
 * - Filament PBR Guide: Material parameters encoding
 * - Real-Time Rendering 4th Ed: Chapter 20 (Efficient G-Buffer layouts)
 */
class AnitoGBuffer {
public:
    AnitoGBuffer(uint32_t width, uint32_t height);
    ~AnitoGBuffer();

    void bind(bgfx::ViewId viewId);
    void resize(uint32_t width, uint32_t height);

    bgfx::FrameBufferHandle getFrameBuffer() const { return m_frameBuffer; }

    // Individual texture access for lighting pass sampling
    bgfx::TextureHandle getAlbedoMetallic() const { return m_rtAlbedoMetallic; }
    bgfx::TextureHandle getNormalRoughness() const { return m_rtNormalRoughness; }
    bgfx::TextureHandle getPositionAO() const { return m_rtPositionAO; }
    bgfx::TextureHandle getEmission() const { return m_rtEmission; }
    bgfx::TextureHandle getDepth() const { return m_rtDepth; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

private:
    void create(uint32_t width, uint32_t height);
    void destroy();

    bgfx::FrameBufferHandle m_frameBuffer;
    uint32_t m_width;
    uint32_t m_height;

    // Individual render target textures (for sampling in lighting pass)
    bgfx::TextureHandle m_rtAlbedoMetallic;   // RT0: RGBA8
    bgfx::TextureHandle m_rtNormalRoughness;  // RT1: RGBA8
    bgfx::TextureHandle m_rtPositionAO;       // RT2: RGBA16F
    bgfx::TextureHandle m_rtEmission;         // RT3: RGBA8
    bgfx::TextureHandle m_rtDepth;            // D24S8
};

} // namespace Anito
