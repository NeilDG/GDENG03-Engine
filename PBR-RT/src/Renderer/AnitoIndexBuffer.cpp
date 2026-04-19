#include "AnitoIndexBuffer.h"
#include <iostream>

namespace Anito {

AnitoIndexBuffer::AnitoIndexBuffer(const void* indices, uint32_t indexCount, bool is32bit)
    : m_indexCount(indexCount)
{
    uint32_t flags = is32bit ? BGFX_BUFFER_INDEX32 : 0;
    uint32_t size = indexCount * (is32bit ? sizeof(uint32_t) : sizeof(uint16_t));

    const bgfx::Memory* mem = bgfx::copy(indices, size);
    m_handle = bgfx::createIndexBuffer(mem, flags);

    if (!bgfx::isValid(m_handle)) {
        std::cerr << "Failed to create index buffer!" << std::endl;
    }
}

AnitoIndexBuffer::~AnitoIndexBuffer() {
    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
    }
}

void AnitoIndexBuffer::bind(uint32_t firstIndex) const {
    bgfx::setIndexBuffer(m_handle, firstIndex, m_indexCount);
}

} // namespace Anito
