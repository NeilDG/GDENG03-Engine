#include "AnitoDeferredRenderer.h"
#include "AnitoGBuffer.h"
#include <iostream>

namespace Anito {

AnitoDeferredRenderer::AnitoDeferredRenderer(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
    , m_geometryViewId(2)  // View 2: Geometry pass (G-Buffer writing)
    , m_lightingViewId(1)  // View 1: Lighting pass (final output)
{
    std::cout << "[AnitoDeferredRenderer] Initializing deferred renderer..." << std::endl;
    m_gBuffer = std::make_unique<AnitoGBuffer>(width, height);
    std::cout << "[AnitoDeferredRenderer] Geometry View ID: " << m_geometryViewId << std::endl;
    std::cout << "[AnitoDeferredRenderer] Lighting View ID: " << m_lightingViewId << std::endl;
    std::cout << "[AnitoDeferredRenderer] Deferred renderer ready!" << std::endl;
}

AnitoDeferredRenderer::~AnitoDeferredRenderer() {
    std::cout << "[AnitoDeferredRenderer] Destroying deferred renderer" << std::endl;
}

void AnitoDeferredRenderer::resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    if (m_gBuffer) {
        m_gBuffer->resize(width, height);
    }
}

void AnitoDeferredRenderer::beginGeometryPass() {
    // Bind G-Buffer framebuffer to geometry view
    // This sets View 2 to render into the G-Buffer render targets
    if (m_gBuffer) {
        m_gBuffer->bind(m_geometryViewId);
        std::cout << "[AnitoDeferredRenderer] Geometry pass started (View " << m_geometryViewId << ")" << std::endl;
    }
}

void AnitoDeferredRenderer::endGeometryPass() {
    // G-Buffer automatically unbound when we bind the next framebuffer
    // or when the frame ends
    std::cout << "[AnitoDeferredRenderer] Geometry pass ended" << std::endl;
}

void AnitoDeferredRenderer::beginLightingPass() {
    // TODO (Step 9): Bind G-Buffer textures for reading, bind backbuffer for writing
    // This will sample from the G-Buffer and render a fullscreen quad with lighting
    std::cout << "[AnitoDeferredRenderer] Lighting pass started (View " << m_lightingViewId << ")" << std::endl;
}

void AnitoDeferredRenderer::endLightingPass() {
    // TODO (Step 9): Complete lighting pass
    std::cout << "[AnitoDeferredRenderer] Lighting pass ended" << std::endl;
}

void AnitoDeferredRenderer::render() {
    // TODO (Step 9): Execute full deferred rendering pipeline
    // 1. Begin geometry pass
    // 2. Render scene objects to G-Buffer
    // 3. End geometry pass
    // 4. Begin lighting pass
    // 5. Render fullscreen triangle with lighting shader
    // 6. End lighting pass
}

} // namespace Anito
