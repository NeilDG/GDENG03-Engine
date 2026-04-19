#include "AnitoGBuffer.h"
#include <iostream>

namespace Anito {

AnitoGBuffer::AnitoGBuffer(uint32_t width, uint32_t height)
    : m_width(width)
    , m_height(height)
{
    create(width, height);
}

AnitoGBuffer::~AnitoGBuffer() {
    destroy();
}

void AnitoGBuffer::create(uint32_t width, uint32_t height) {
    // TODO: Create G-Buffer render targets
    // For now, using BGFX_INVALID_HANDLE as placeholder
    m_frameBuffer = BGFX_INVALID_HANDLE;

    std::cout << "AnitoGBuffer created (stub): " << width << "x" << height << std::endl;
}

void AnitoGBuffer::destroy() {
    if (bgfx::isValid(m_frameBuffer)) {
        bgfx::destroy(m_frameBuffer);
    }
}

void AnitoGBuffer::bind() {
    // TODO: Bind framebuffer for geometry pass rendering
}

void AnitoGBuffer::resize(uint32_t width, uint32_t height) {
    if (m_width != width || m_height != height) {
        destroy();
        create(width, height);
        m_width = width;
        m_height = height;
    }
}

} // namespace Anito
