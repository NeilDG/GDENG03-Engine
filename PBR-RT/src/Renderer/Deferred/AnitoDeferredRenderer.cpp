#include "AnitoDeferredRenderer.h"
#include "AnitoGBuffer.h"

namespace Anito {

AnitoDeferredRenderer::AnitoDeferredRenderer(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    m_gBuffer = std::make_unique<AnitoGBuffer>(width, height);
}

AnitoDeferredRenderer::~AnitoDeferredRenderer() {
}

void AnitoDeferredRenderer::resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    if (m_gBuffer) {
        m_gBuffer->resize(width, height);
    }
}

void AnitoDeferredRenderer::beginGeometryPass() {
    // TODO: Bind G-Buffer and set up for geometry rendering
}

void AnitoDeferredRenderer::endGeometryPass() {
    // TODO: Unbind G-Buffer
}

void AnitoDeferredRenderer::beginLightingPass() {
    // TODO: Bind G-Buffer textures for reading, bind backbuffer for writing
}

void AnitoDeferredRenderer::endLightingPass() {
    // TODO: Complete lighting pass
}

void AnitoDeferredRenderer::render() {
    // TODO: Execute full deferred rendering pipeline
}

} // namespace Anito
