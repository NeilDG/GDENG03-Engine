#include "AnitoDeferredRenderer.h"
#include "AnitoGBuffer.h"
#include <iostream>
#include <cstring>

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
    m_gBuffer.reset();
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
    // Bind backbuffer for output (View 1)
    bgfx::setViewFrameBuffer(m_lightingViewId, BGFX_INVALID_HANDLE);

    // Set view rectangle to full screen
    bgfx::setViewRect(m_lightingViewId, 0, 0, m_width, m_height);

    // Preserve skybox color from earlier view; clear depth only
    bgfx::setViewClear(m_lightingViewId,
        BGFX_CLEAR_DEPTH,
        0x000000ff,
        1.0f,
        0);

    // Identity view and projection: the vertex shader positions directly in clip space
    float identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    bgfx::setViewTransform(m_lightingViewId, identity, identity);
}

void AnitoDeferredRenderer::endLightingPass() {
    // Render fullscreen triangle with lighting shader
    if (!bgfx::isValid(m_lightingShader)) {
        std::cerr << "[AnitoDeferredRenderer] ERROR: Lighting shader not set!" << std::endl;
        return;
    }

    // Build a 2-float-position vertex layout matching varying_deferred_light.def.sc
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .end();

    // Fullscreen triangle in NDC — covers the entire viewport with 3 vertices
    // Reference: "A trip through the Graphics Pipeline" - Jon Olick, GDC 2011
    struct PosVertex2D { float x, y; };
    static const PosVertex2D kFullscreenTriangle[3] = {
        { -1.0f, -1.0f },
        {  3.0f, -1.0f },
        { -1.0f,  3.0f },
    };

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, 3, layout);
    
    if (tvb.data == nullptr) {
        std::cerr << "[AnitoDeferredRenderer] ERROR: Failed to allocate transient vertex buffer for fullscreen triangle!" << std::endl;
        return;
    }
    memcpy(tvb.data, kFullscreenTriangle, sizeof(kFullscreenTriangle));
    bgfx::setVertexBuffer(0, &tvb);

    // Bind G-Buffer textures for sampling
    if (m_gBuffer) {
        bgfx::setTexture(0, m_s_gbuffer0, m_gBuffer->getAlbedoMetallic());   // Albedo + Metallic
        bgfx::setTexture(1, m_s_gbuffer1, m_gBuffer->getNormalRoughness());  // Normal + Roughness
        bgfx::setTexture(2, m_s_gbuffer2, m_gBuffer->getPositionAO());       // Position + AO
        bgfx::setTexture(3, m_s_gbuffer3, m_gBuffer->getEmission());         // Emission
    }

    // Fullscreen pass: write RGB+A, blend with existing skybox via alpha,
    // no depth test (we cover the whole screen regardless of depth).
    uint64_t state = 0
        | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_DEPTH_TEST_ALWAYS
        | BGFX_STATE_BLEND_ALPHA;

    bgfx::setState(state);

    bgfx::submit(m_lightingViewId, m_lightingShader);

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

void AnitoDeferredRenderer::setLightingShader(bgfx::ProgramHandle shader) {
    m_lightingShader = shader;
    std::cout << "[AnitoDeferredRenderer] Lighting shader set" << std::endl;
}

void AnitoDeferredRenderer::setDebugShader(bgfx::ProgramHandle shader) {
    m_debugShader = shader;
    std::cout << "[AnitoDeferredRenderer] Debug visualization shader set" << std::endl;
}

void AnitoDeferredRenderer::setGBufferUniforms(bgfx::UniformHandle s_gbuffer0, bgfx::UniformHandle s_gbuffer1,
                                               bgfx::UniformHandle s_gbuffer2, bgfx::UniformHandle s_gbuffer3) {
    m_s_gbuffer0 = s_gbuffer0;
    m_s_gbuffer1 = s_gbuffer1;
    m_s_gbuffer2 = s_gbuffer2;
    m_s_gbuffer3 = s_gbuffer3;
    std::cout << "[AnitoDeferredRenderer] G-Buffer uniforms set" << std::endl;
}

void AnitoDeferredRenderer::setCameraUniforms(bgfx::UniformHandle u_cameraPos) {
    m_u_cameraPos = u_cameraPos;
    std::cout << "[AnitoDeferredRenderer] Camera uniforms set" << std::endl;
}

void AnitoDeferredRenderer::setIBLUniforms(bgfx::UniformHandle s_irradianceMap, bgfx::UniformHandle s_prefilterMap, bgfx::UniformHandle s_brdfLUT) {
    m_s_irradianceMap = s_irradianceMap;
    m_s_prefilterMap = s_prefilterMap;
    m_s_brdfLUT = s_brdfLUT;
    std::cout << "[AnitoDeferredRenderer] IBL uniforms set" << std::endl;
}

void AnitoDeferredRenderer::renderDebugVisualization() {
    // [STEP 10] Render G-Buffer debug visualization (split-screen view)
    if (!bgfx::isValid(m_debugShader)) {
        std::cerr << "[AnitoDeferredRenderer] ERROR: Debug shader not set!" << std::endl;
        return;
    }

    // Bind backbuffer for output
    bgfx::setViewFrameBuffer(m_lightingViewId, BGFX_INVALID_HANDLE);
    bgfx::setViewRect(m_lightingViewId, 0, 0, m_width, m_height);

    // Set clear flags
    bgfx::setViewClear(m_lightingViewId,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x000000ff,
        1.0f,
        0);

    // Set identity transform (fullscreen pass)
    float identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    bgfx::setViewTransform(m_lightingViewId, identity, identity);

    // Bind G-Buffer textures for sampling
    if (m_gBuffer) {
        bgfx::setTexture(0, m_s_gbuffer0, m_gBuffer->getAlbedoMetallic());
        bgfx::setTexture(1, m_s_gbuffer1, m_gBuffer->getNormalRoughness());
        bgfx::setTexture(2, m_s_gbuffer2, m_gBuffer->getPositionAO());
        bgfx::setTexture(3, m_s_gbuffer3, m_gBuffer->getEmission());
    }

    // Set render state
    uint64_t state = 0
        | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_WRITE_A;

    bgfx::setState(state);

    // Submit fullscreen triangle with debug shader
    bgfx::submit(m_lightingViewId, m_debugShader);

    std::cout << "[AnitoDeferredRenderer] Debug visualization rendered" << std::endl;
}

} // namespace Anito
