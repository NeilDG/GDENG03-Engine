#pragma once
// Shadow mapping system

#include <bgfx/bgfx.h>
#include <cstdint>

namespace Anito {

class AnitoLight;

/**
 * AnitoShadowMap - Shadow map management
 * Supports cascaded shadow maps for directional lights
 * and standard shadow maps for point/spot lights
 */
class AnitoShadowMap {
public:
    AnitoShadowMap(uint32_t resolution = 2048);
    ~AnitoShadowMap();

    void beginShadowPass(const AnitoLight* light);
    void endShadowPass();

    bgfx::TextureHandle getShadowTexture() const { return m_shadowTexture; }
    uint32_t getResolution() const { return m_resolution; }

private:
    void createShadowMap(uint32_t resolution);
    void destroyShadowMap();

    bgfx::FrameBufferHandle m_shadowFrameBuffer;
    bgfx::TextureHandle m_shadowTexture;
    uint32_t m_resolution;
};

} // namespace Anito
