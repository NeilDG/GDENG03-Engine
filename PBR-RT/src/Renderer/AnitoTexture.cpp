#include "AnitoTexture.h"
#include <bimg/decode.h>
#include <bx/readerwriter.h>
#include <bx/file.h>
#include <iostream>

namespace Anito {

// Initialize static members
std::unordered_map<std::string, std::shared_ptr<AnitoTexture>> AnitoTexture::s_textureCache;
std::shared_ptr<AnitoTexture> AnitoTexture::s_whiteTexture;
std::shared_ptr<AnitoTexture> AnitoTexture::s_blackTexture;
std::shared_ptr<AnitoTexture> AnitoTexture::s_normalTexture;

AnitoTexture::AnitoTexture(const std::string& name)
    : m_name(name)
    , m_handle(BGFX_INVALID_HANDLE)
    , m_width(0)
    , m_height(0)
    , m_format(Format::RGBA8)
{
}

AnitoTexture::~AnitoTexture() {
    if (bgfx::isValid(m_handle)) {
        bgfx::destroy(m_handle);
    }
}

bool AnitoTexture::loadFromFile(const std::string& path, Format format) {
    bx::DefaultAllocator defaultAllocator;
    bx::FileReader reader;

    if (bx::open(&reader, path.c_str())) {
        uint32_t size = (uint32_t)bx::getSize(&reader);
        const bgfx::Memory* mem = bgfx::alloc(size + 1);
        bx::read(&reader, mem->data, size, bx::ErrorAssert{});
        bx::close(&reader);

        mem->data[mem->size - 1] = '\0';

        // Decode image
        bimg::ImageContainer* imageContainer = bimg::imageParse(&defaultAllocator, mem->data, size);

        if (imageContainer != nullptr) {
            const bgfx::Memory* imgMem = bgfx::makeRef(
                imageContainer->m_data,
                imageContainer->m_size,
                [](void* _ptr, void* _userData) {
                    bimg::ImageContainer* imgContainer = (bimg::ImageContainer*)_userData;
                    bimg::imageFree(imgContainer);
                },
                imageContainer
            );

            m_width = imageContainer->m_width;
            m_height = imageContainer->m_height;

            // Determine format
            bgfx::TextureFormat::Enum texFormat = bgfx::TextureFormat::Enum(imageContainer->m_format);

            uint64_t flags = BGFX_TEXTURE_NONE
                | BGFX_SAMPLER_MIN_ANISOTROPIC
                | BGFX_SAMPLER_MAG_ANISOTROPIC
                | BGFX_SAMPLER_MIP_POINT;

            m_handle = bgfx::createTexture2D(
                uint16_t(m_width),
                uint16_t(m_height),
                imageContainer->m_numMips > 1,
                imageContainer->m_numLayers,
                texFormat,
                flags,
                imgMem
            );

            if (bgfx::isValid(m_handle)) {
                std::cout << "[AnitoTexture] Loaded texture: " << path << " (" << m_width << "x" << m_height << ")" << std::endl;
                return true;
            }
        }
    }

    std::cerr << "[AnitoTexture] Failed to load texture: " << path << std::endl;
    return false;
}

bool AnitoTexture::createFromMemory(const void* data, uint32_t width, uint32_t height, Format format) {
    m_width = width;
    m_height = height;
    m_format = format;

    bgfx::TextureFormat::Enum bgfxFormat = formatToBgfx(format);
    uint64_t flags = getDefaultFlags(format);

    // Calculate data size based on format
    uint32_t bpp = 4; // bytes per pixel for RGBA8
    if (format == Format::RGB8) bpp = 3;
    else if (format == Format::R8) bpp = 1;
    else if (format == Format::RGBA16F) bpp = 8;
    else if (format == Format::RGBA32F) bpp = 16;

    const bgfx::Memory* mem = bgfx::copy(data, width * height * bpp);

    m_handle = bgfx::createTexture2D(
        uint16_t(width),
        uint16_t(height),
        false,
        1,
        bgfxFormat,
        flags,
        mem
    );

    return bgfx::isValid(m_handle);
}

bool AnitoTexture::createEmpty(uint32_t width, uint32_t height, Format format, uint64_t flags) {
    m_width = width;
    m_height = height;
    m_format = format;

    bgfx::TextureFormat::Enum bgfxFormat = formatToBgfx(format);

    if (flags == 0) {
        flags = getDefaultFlags(format);
    }

    m_handle = bgfx::createTexture2D(
        uint16_t(width),
        uint16_t(height),
        false,
        1,
        bgfxFormat,
        flags,
        nullptr
    );

    return bgfx::isValid(m_handle);
}

void AnitoTexture::setWrapMode(WrapMode mode) {
    // Note: Wrap mode is set via flags in bgfx, would need to recreate texture
    // For now, this is a placeholder
}

void AnitoTexture::setFilterMode(FilterMode mode) {
    // Note: Filter mode is set via flags in bgfx, would need to recreate texture
    // For now, this is a placeholder
}

bgfx::TextureFormat::Enum AnitoTexture::formatToBgfx(Format format) {
    switch (format) {
        case Format::RGBA8:  return bgfx::TextureFormat::RGBA8;
        case Format::RGB8:   return bgfx::TextureFormat::RGB8;
        case Format::R8:     return bgfx::TextureFormat::R8;
        case Format::RGBA16F: return bgfx::TextureFormat::RGBA16F;
        case Format::RGBA32F: return bgfx::TextureFormat::RGBA32F;
        case Format::Depth24Stencil8: return bgfx::TextureFormat::D24S8;
        default: return bgfx::TextureFormat::RGBA8;
    }
}

uint64_t AnitoTexture::getDefaultFlags(Format format) {
    uint64_t flags = BGFX_TEXTURE_NONE;

    if (format == Format::Depth24Stencil8) {
        flags |= BGFX_TEXTURE_RT;
    } else {
        flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
    }

    return flags;
}

std::shared_ptr<AnitoTexture> AnitoTexture::getWhiteTexture() {
    if (!s_whiteTexture) {
        s_whiteTexture = std::make_shared<AnitoTexture>("White");
        uint32_t whitePixel = 0xFFFFFFFF;
        s_whiteTexture->createFromMemory(&whitePixel, 1, 1, Format::RGBA8);
    }
    return s_whiteTexture;
}

std::shared_ptr<AnitoTexture> AnitoTexture::getBlackTexture() {
    if (!s_blackTexture) {
        s_blackTexture = std::make_shared<AnitoTexture>("Black");
        uint32_t blackPixel = 0xFF000000;
        s_blackTexture->createFromMemory(&blackPixel, 1, 1, Format::RGBA8);
    }
    return s_blackTexture;
}

std::shared_ptr<AnitoTexture> AnitoTexture::getNormalTexture() {
    if (!s_normalTexture) {
        s_normalTexture = std::make_shared<AnitoTexture>("Normal");
        uint32_t normalPixel = 0xFFFF8080; // Normal pointing up (0.5, 0.5, 1.0)
        s_normalTexture->createFromMemory(&normalPixel, 1, 1, Format::RGBA8);
    }
    return s_normalTexture;
}

std::shared_ptr<AnitoTexture> AnitoTexture::createOrGet(const std::string& path) {
    auto it = s_textureCache.find(path);
    if (it != s_textureCache.end()) {
        return it->second;
    }

    auto texture = std::make_shared<AnitoTexture>(path);
    if (!texture->loadFromFile(path)) {
        return nullptr;
    }

    s_textureCache[path] = texture;
    return texture;
}

void AnitoTexture::clearCache() {
    s_textureCache.clear();
    s_whiteTexture.reset();
    s_blackTexture.reset();
    s_normalTexture.reset();
}

} // namespace Anito
