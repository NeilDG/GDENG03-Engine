#include "AnitoMeshRenderer.h"
#include "AnitoTransform.h"
#include "../Renderer/AnitoVertexBuffer.h"
#include "../Renderer/AnitoIndexBuffer.h"
#include "../Renderer/AnitoMaterial.h"
#include "../Renderer/AnitoRenderer.h"
#include "../Renderer/AnitoShader.h"
#include "../GameObjects/AnitoGameObject.h"
#include <bgfx/bgfx.h>

namespace Anito {

AnitoMeshRenderer::AnitoMeshRenderer(const std::string& name)
    : AnitoComponent(name, ComponentType::MeshRenderer)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
    , m_material(nullptr)
{
}

AnitoMeshRenderer::~AnitoMeshRenderer() {
    // Buffers and materials are managed by shared_ptr
}

void AnitoMeshRenderer::render() {
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    // Use main view for forward rendering
    renderToView(renderer->getMainViewId());
}

void AnitoMeshRenderer::renderToView(bgfx::ViewId viewId) {
    renderToView(viewId, nullptr);
}

void AnitoMeshRenderer::renderToView(bgfx::ViewId viewId, const std::shared_ptr<AnitoShader>& overrideShader) {
    if (!m_vertexBuffer || !m_indexBuffer || !m_material) return;

    std::shared_ptr<AnitoShader> shaderToUse = overrideShader ? overrideShader : m_material->getShader();
    if (!shaderToUse || !shaderToUse->isValid()) return;

    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    // Set transform matrices from owner GameObject
    if (m_owner) {
        // Use GameObject's local matrix directly
        bgfx::setTransform(m_owner->getMatrixData());
    }

    // Bind vertex and index buffers
    m_vertexBuffer->bind();
    m_indexBuffer->bind();

    // Bind material parameters (still needed for gbuffer uniforms)
    m_material->bind(viewId);

    // CRITICAL: Bind IBL textures per-draw-call (bgfx requirement)
    // Only for forward rendering path using material shader
    if (!overrideShader && viewId == renderer->getMainViewId() && renderer->isIBLReady()) {
        renderer->bindIBLTextures(m_material->getShader());
    }

    // Set render state (default for now)
    uint64_t state = 0
        | BGFX_STATE_WRITE_RGB
        | BGFX_STATE_WRITE_A
        | BGFX_STATE_WRITE_Z
        | BGFX_STATE_DEPTH_TEST_LESS
        | BGFX_STATE_CULL_CW
        | BGFX_STATE_MSAA;

    bgfx::setState(state);

    // Submit draw call to the specified view
    bgfx::submit(viewId, shaderToUse->getProgram());
}

} // namespace Anito
