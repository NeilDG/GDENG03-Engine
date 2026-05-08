#include "AnitoGBuffer.h"
#include <iostream>
#include <array>

namespace Anito {

AnitoGBuffer::AnitoGBuffer(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_frameBuffer(BGFX_INVALID_HANDLE)
    , m_rtAlbedoMetallic(BGFX_INVALID_HANDLE)
    , m_rtNormalRoughness(BGFX_INVALID_HANDLE)
    , m_rtPositionAO(BGFX_INVALID_HANDLE)
    , m_rtEmission(BGFX_INVALID_HANDLE)
    , m_rtDepth(BGFX_INVALID_HANDLE)
{
    create(width, height);
}

AnitoGBuffer::~AnitoGBuffer() {
    destroy();
}

void AnitoGBuffer::create(uint32_t width, uint32_t height) {
    std::cout << "[AnitoGBuffer] Creating G-Buffer: " << width << "x" << height << std::endl;

    // Create render target textures following mobile-friendly format guidelines
    // Reference: BGFX example 21-deferred, Filament material encoding

    // RT0: Albedo (RGB, sRGB) + Metallic (A, linear)
    // Using RGBA8 for memory efficiency (4 bytes/pixel)
    m_rtAlbedoMetallic = bgfx::createTexture2D(
        uint16_t(width), uint16_t(height),
        false, // no mipmaps
        1,     // 1 layer
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    if (!bgfx::isValid(m_rtAlbedoMetallic)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create Albedo+Metallic RT!" << std::endl;
        return;
    }
    std::cout << "[AnitoGBuffer]   RT0: Albedo+Metallic (RGBA8) created" << std::endl;

    // RT1: Normal (RGB, encoded to [0,1]) + Roughness (A, linear)
    // Using RGBA8 - sufficient precision for normals with proper encoding
    m_rtNormalRoughness = bgfx::createTexture2D(
        uint16_t(width), uint16_t(height),
        false, // no mipmaps
        1,     // 1 layer
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    if (!bgfx::isValid(m_rtNormalRoughness)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create Normal+Roughness RT!" << std::endl;
        destroy();
        return;
    }
    std::cout << "[AnitoGBuffer]   RT1: Normal+Roughness (RGBA8) created" << std::endl;

    // RT2: World Position (RGB, requires float precision) + AO (A, linear)
    // Using RGBA16F (8 bytes/pixel) - essential for accurate position reconstruction
    // Alternative: Could use view-space depth reconstruction to save bandwidth
    m_rtPositionAO = bgfx::createTexture2D(
        uint16_t(width), uint16_t(height),
        false, // no mipmaps
        1,     // 1 layer
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    if (!bgfx::isValid(m_rtPositionAO)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create Position+AO RT!" << std::endl;
        destroy();
        return;
    }
    std::cout << "[AnitoGBuffer]   RT2: Position+AO (RGBA16F) created" << std::endl;

    // RT3: Emission (RGB, HDR encoded to LDR) + Unused (A)
    // Using RGBA8 for memory efficiency - emissions can be scaled/encoded
    m_rtEmission = bgfx::createTexture2D(
        uint16_t(width), uint16_t(height),
        false, // no mipmaps
        1,     // 1 layer
        bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    if (!bgfx::isValid(m_rtEmission)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create Emission RT!" << std::endl;
        destroy();
        return;
    }
    std::cout << "[AnitoGBuffer]   RT3: Emission (RGBA8) created" << std::endl;

    // Depth/Stencil buffer: D24S8 (standard 24-bit depth + 8-bit stencil)
    m_rtDepth = bgfx::createTexture2D(
        uint16_t(width), uint16_t(height),
        false, // no mipmaps
        1,     // 1 layer
        bgfx::TextureFormat::D24S8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );

    if (!bgfx::isValid(m_rtDepth)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create Depth RT!" << std::endl;
        destroy();
        return;
    }
    std::cout << "[AnitoGBuffer]   Depth: D24S8 created" << std::endl;

    // Create framebuffer with all attachments
    // Order: RT0, RT1, RT2, RT3, Depth
    std::array<bgfx::Attachment, 5> attachments;
    attachments[0].init(m_rtAlbedoMetallic, bgfx::Access::Write);
    attachments[1].init(m_rtNormalRoughness, bgfx::Access::Write);
    attachments[2].init(m_rtPositionAO, bgfx::Access::Write);
    attachments[3].init(m_rtEmission, bgfx::Access::Write);
    attachments[4].init(m_rtDepth, bgfx::Access::Write);

    m_frameBuffer = bgfx::createFrameBuffer(
        uint8_t(attachments.size()),
        attachments.data(),
        false // don't destroy textures when framebuffer is destroyed
    );

    if (!bgfx::isValid(m_frameBuffer)) {
        std::cerr << "[AnitoGBuffer] ERROR: Failed to create framebuffer!" << std::endl;
        destroy();
        return;
    }

    std::cout << "[AnitoGBuffer] ✅ G-Buffer framebuffer created successfully" << std::endl;
    std::cout << "[AnitoGBuffer] Memory usage (approximate): " << std::endl;
    std::cout << "[AnitoGBuffer]   - 3x RGBA8 (RT0,1,3): " << (width * height * 4 * 3) / (1024*1024) << " MB" << std::endl;
    std::cout << "[AnitoGBuffer]   - 1x RGBA16F (RT2): " << (width * height * 8) / (1024*1024) << " MB" << std::endl;
    std::cout << "[AnitoGBuffer]   - 1x D24S8 (Depth): " << (width * height * 4) / (1024*1024) << " MB" << std::endl;
    std::cout << "[AnitoGBuffer]   - Total: ~" << (width * height * 24) / (1024*1024) << " MB" << std::endl;
}

void AnitoGBuffer::destroy() {
    if (bgfx::isValid(m_frameBuffer)) {
        bgfx::destroy(m_frameBuffer);
        m_frameBuffer = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(m_rtAlbedoMetallic)) {
        bgfx::destroy(m_rtAlbedoMetallic);
        m_rtAlbedoMetallic = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_rtNormalRoughness)) {
        bgfx::destroy(m_rtNormalRoughness);
        m_rtNormalRoughness = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_rtPositionAO)) {
        bgfx::destroy(m_rtPositionAO);
        m_rtPositionAO = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_rtEmission)) {
        bgfx::destroy(m_rtEmission);
        m_rtEmission = BGFX_INVALID_HANDLE;
    }
    if (bgfx::isValid(m_rtDepth)) {
        bgfx::destroy(m_rtDepth);
        m_rtDepth = BGFX_INVALID_HANDLE;
    }

    std::cout << "[AnitoGBuffer] G-Buffer destroyed" << std::endl;
}

void AnitoGBuffer::bind(bgfx::ViewId viewId) {
    // Set this G-Buffer as the render target for the specified view
    bgfx::setViewFrameBuffer(viewId, m_frameBuffer);

    // Clear all render targets
    // Clear color RTs to black, depth to 1.0
    bgfx::setViewClear(viewId,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL,
        0x00000000, // Black clear color
        1.0f,       // Far depth
        0           // Stencil 0
    );

    // Set viewport
    bgfx::setViewRect(viewId, 0, 0, uint16_t(m_width), uint16_t(m_height));
}

void AnitoGBuffer::resize(uint32_t width, uint32_t height) {
    if (m_width != width || m_height != height) {
        std::cout << "[AnitoGBuffer] Resizing from " << m_width << "x" << m_height 
                  << " to " << width << "x" << height << std::endl;
        destroy();
        create(width, height);
        m_width = width;
        m_height = height;
    }
}

} // namespace Anito
