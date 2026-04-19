#include "AnitoShadowMap.h"
#include "AnitoLight.h"
#include <iostream>

namespace Anito {

AnitoShadowMap::AnitoShadowMap(uint32_t resolution)
    : m_resolution(resolution)
{
    createShadowMap(resolution);
}

AnitoShadowMap::~AnitoShadowMap() {
    destroyShadowMap();
}

void AnitoShadowMap::createShadowMap(uint32_t resolution) {
    // TODO: Create shadow map framebuffer and depth texture
    m_shadowFrameBuffer = BGFX_INVALID_HANDLE;
    m_shadowTexture = BGFX_INVALID_HANDLE;

    std::cout << "AnitoShadowMap created (stub): " << resolution << "x" << resolution << std::endl;
}

void AnitoShadowMap::destroyShadowMap() {
    if (bgfx::isValid(m_shadowFrameBuffer)) {
        bgfx::destroy(m_shadowFrameBuffer);
    }
    if (bgfx::isValid(m_shadowTexture)) {
        bgfx::destroy(m_shadowTexture);
    }
}

void AnitoShadowMap::beginShadowPass(const AnitoLight* light) {
    // TODO: Set up shadow rendering
    // - Bind shadow framebuffer
    // - Set appropriate view/projection matrices
    // - Clear depth buffer
}

void AnitoShadowMap::endShadowPass() {
    // TODO: Complete shadow rendering
}

} // namespace Anito
