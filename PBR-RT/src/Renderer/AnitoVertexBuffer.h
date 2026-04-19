#pragma once
#include <bgfx/bgfx.h>
#include <vector>

namespace Anito {

/**
 * AnitoVertexBuffer - Wrapper around bgfx vertex buffer
 */
class AnitoVertexBuffer {
public:
    AnitoVertexBuffer(const void* vertices, uint32_t vertexCount, const bgfx::VertexLayout& layout);
    ~AnitoVertexBuffer();

    void bind(uint8_t stream = 0, uint32_t startVertex = 0) const;
    bgfx::VertexBufferHandle getHandle() const { return m_handle; }
    uint32_t getVertexCount() const { return m_vertexCount; }

private:
    bgfx::VertexBufferHandle m_handle;
    uint32_t m_vertexCount;
};

// Common vertex layouts
struct PosColorVertex {
    float x, y, z;
    uint32_t abgr;

    static void init();
    static bgfx::VertexLayout ms_layout;
};

struct PosNormalTexcoordVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    uint32_t abgr;  // Vertex color (defaultwhite: 0xFFFFFFFF)

    static void init();
    static bgfx::VertexLayout ms_layout;
};

} // namespace Anito
