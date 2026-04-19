#include "AnitoVertexBuffer.h"
#include <iostream>

namespace Anito {

// Initialize static layouts
bgfx::VertexLayout PosColorVertex::ms_layout;
bgfx::VertexLayout PosNormalTexcoordVertex::ms_layout;

void PosColorVertex::init() {
    ms_layout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
}

void PosNormalTexcoordVertex::init() {
    ms_layout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
}

AnitoVertexBuffer::AnitoVertexBuffer(const void* vertices, uint32_t vertexCount, const bgfx::VertexLayout& layout)
    : m_vertexCount(vertexCount)
{
    const bgfx::Memory* mem = bgfx::copy(vertices, vertexCount * layout.getStride());
    m_handle = bgfx::createVertexBuffer(mem, layout);

    if (!bgfx::isValid(m_handle)) {
        std::cerr << "Failed to create vertex buffer!" << std::endl;
    }
}

AnitoVertexBuffer::~AnitoVertexBuffer() {
    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
    }
}

void AnitoVertexBuffer::bind(uint8_t stream, uint32_t startVertex) const {
    bgfx::setVertexBuffer(stream, m_handle, startVertex, m_vertexCount);
}

} // namespace Anito
