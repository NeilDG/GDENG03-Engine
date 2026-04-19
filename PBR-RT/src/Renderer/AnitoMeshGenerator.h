#pragma once
#include "AnitoVertexBuffer.h"
#include "AnitoIndexBuffer.h"
#include <memory>

namespace Anito {

/**
 * AnitoMeshGenerator - Procedural mesh generation utilities
 * Creates common geometric shapes for testing and prototyping
 */
class AnitoMeshGenerator {
public:
    struct MeshData {
        std::shared_ptr<AnitoVertexBuffer> vertexBuffer;
        std::shared_ptr<AnitoIndexBuffer> indexBuffer;
    };

    // Primitive generators
    static MeshData createCube(float size = 1.0f);
    static MeshData createSphere(float radius = 1.0f, uint32_t segments = 32, uint32_t rings = 16);
    static MeshData createPlane(float width = 1.0f, float height = 1.0f, uint32_t subdivisionsX = 1, uint32_t subdivisionsY = 1);
    static MeshData createCylinder(float radius = 1.0f, float height = 2.0f, uint32_t segments = 32);
    static MeshData createCone(float radius = 1.0f, float height = 2.0f, uint32_t segments = 32);

    // Screen quad for post-processing
    static MeshData createScreenQuad();
};

} // namespace Anito
