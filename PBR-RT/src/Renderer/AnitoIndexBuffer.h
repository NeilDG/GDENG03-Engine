#pragma once
#include <bgfx/bgfx.h>
#include <cstdint>

namespace Anito {

/**
 * AnitoIndexBuffer - Wrapper around bgfx index buffer
 */
class AnitoIndexBuffer {
public:
    AnitoIndexBuffer(const void* indices, uint32_t indexCount, bool is32bit = false);
    ~AnitoIndexBuffer();

    void bind(uint32_t firstIndex = 0) const;
    bgfx::IndexBufferHandle getHandle() const { return m_handle; }
    uint32_t getIndexCount() const { return m_indexCount; }

private:
    bgfx::IndexBufferHandle m_handle;
    uint32_t m_indexCount;
};

} // namespace Anito
