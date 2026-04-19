#pragma once
// Mesh Renderer component - renders a mesh

#include "../GameObjects/AnitoComponent.h"
#include <bgfx/bgfx.h>
#include <memory>

namespace Anito {

class AnitoVertexBuffer;
class AnitoIndexBuffer;
class AnitoMaterial;

/**
 * AnitoMeshRenderer - Renders a mesh with a material
 */
class AnitoMeshRenderer : public AnitoComponent {
public:
    AnitoMeshRenderer(const std::string& name = "MeshRenderer");
    ~AnitoMeshRenderer() override;

    void render() override;

    void setVertexBuffer(std::shared_ptr<AnitoVertexBuffer> vb) { m_vertexBuffer = vb; }
    void setIndexBuffer(std::shared_ptr<AnitoIndexBuffer> ib) { m_indexBuffer = ib; }
    void setMaterial(std::shared_ptr<AnitoMaterial> mat) { m_material = mat; }

private:
    std::shared_ptr<AnitoVertexBuffer> m_vertexBuffer;
    std::shared_ptr<AnitoIndexBuffer> m_indexBuffer;
    std::shared_ptr<AnitoMaterial> m_material;
};

} // namespace Anito
