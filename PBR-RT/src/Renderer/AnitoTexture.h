#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace Anito {

/**
 * AnitoTexture - Texture loading and management
 * Supports various image formats via STB image
 */
class AnitoTexture {
public:
    enum class Format {
        RGBA8,
        RGB8,
        R8,
        RGBA16F,
        RGBA32F,
        Depth24Stencil8,
        Auto  // Auto-detect from image
    };

    enum class WrapMode {
        Repeat,
        Clamp,
        Mirror
    };

    enum class FilterMode {
        Point,
        Linear,
        Anisotropic
    };

public:
    AnitoTexture(const std::string& name = "Texture");
    ~AnitoTexture();

    // Loading from file
    bool loadFromFile(const std::string& path, Format format = Format::Auto);

    // Create from memory
    bool createFromMemory(const void* data, uint32_t width, uint32_t height, Format format);

    // Create empty texture (for render targets)
    bool createEmpty(uint32_t width, uint32_t height, Format format, uint64_t flags = 0);

    // Accessors
    bool isValid() const { return bgfx::isValid(m_handle); }
    bgfx::TextureHandle getHandle() const { return m_handle; }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    Format getFormat() const { return m_format; }

    const std::string& getName() const { return m_name; }

    // Sampler settings
    void setWrapMode(WrapMode mode);
    void setFilterMode(FilterMode mode);

    // Static default textures
    static std::shared_ptr<AnitoTexture> getWhiteTexture();
    static std::shared_ptr<AnitoTexture> getBlackTexture();
    static std::shared_ptr<AnitoTexture> getNormalTexture();

    // Texture cache
    static std::shared_ptr<AnitoTexture> createOrGet(const std::string& path);
    static void clearCache();

private:
    static bgfx::TextureFormat::Enum formatToBgfx(Format format);
    static uint64_t getDefaultFlags(Format format);

    std::string m_name;
    bgfx::TextureHandle m_handle;
    uint32_t m_width;
    uint32_t m_height;
    Format m_format;

    // Texture cache
    static std::unordered_map<std::string, std::shared_ptr<AnitoTexture>> s_textureCache;
    static std::shared_ptr<AnitoTexture> s_whiteTexture;
    static std::shared_ptr<AnitoTexture> s_blackTexture;
    static std::shared_ptr<AnitoTexture> s_normalTexture;
};

} // namespace Anito
