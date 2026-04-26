#include "AnitoMeshRenderer.h"
#include "AnitoTransform.h"
#include "../Renderer/AnitoVertexBuffer.h"
#include "../Renderer/AnitoIndexBuffer.h"
#include "../Renderer/AnitoMaterial.h"
#include "../Renderer/AnitoRenderer.h"
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
    if (!m_vertexBuffer || !m_indexBuffer || !m_material) return;
    if (!m_material->getShader() || !m_material->getShader()->isValid()) return;

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

    // Bind material (PBR parameters)
    m_material->bind(renderer->getMainViewId());

    // CRITICAL: Bind IBL textures per-draw-call (bgfx requirement)
    // This must be done AFTER material bind and BEFORE setState/submit
    if (renderer->isIBLReady()) {
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

    // Submit draw call
    bgfx::submit(renderer->getMainViewId(), m_material->getShader()->getProgram());
}

} // namespace Anito
