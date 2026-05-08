#include "AnitoMaterial.h"
#include <iostream>

namespace Anito {

AnitoMaterial::AnitoMaterial(const std::string& name)
    : m_name(name)
    , m_baseColor{1.0f, 1.0f, 1.0f, 1.0f}
    , m_metallic(0.0f)
    , m_roughness(0.5f)
    , m_ao(1.0f)
    , m_emissive{0.0f, 0.0f, 0.0f}
    , m_uniformBaseColor(BGFX_INVALID_HANDLE)
    , m_uniformPbrParams(BGFX_INVALID_HANDLE)
    , m_uniformEmissive(BGFX_INVALID_HANDLE)
{
    // Initialize texture slots with defaults
    for (int i = 0; i < static_cast<int>(TextureSlot::Count); ++i) {
        m_textures[i] = nullptr;
    }

    setupDefaultUniforms();
}

AnitoMaterial::~AnitoMaterial()
{
    if (bgfx::isValid(m_uniformBaseColor)) { bgfx::destroy(m_uniformBaseColor); m_uniformBaseColor = BGFX_INVALID_HANDLE; }
    if (bgfx::isValid(m_uniformPbrParams)) { bgfx::destroy(m_uniformPbrParams); m_uniformPbrParams = BGFX_INVALID_HANDLE; }
    if (bgfx::isValid(m_uniformEmissive)) { bgfx::destroy(m_uniformEmissive); m_uniformEmissive = BGFX_INVALID_HANDLE; }
}

void AnitoMaterial::setupDefaultUniforms() {
    if (!bgfx::isValid(m_uniformBaseColor))
    {
        m_uniformBaseColor = bgfx::createUniform("u_baseColor", bgfx::UniformType::Vec4);
    }
    if (!bgfx::isValid(m_uniformPbrParams))
    {
        m_uniformPbrParams = bgfx::createUniform("u_pbrParams", bgfx::UniformType::Vec4);
    }
    if (!bgfx::isValid(m_uniformEmissive))
    {
        m_uniformEmissive = bgfx::createUniform("u_emissive", bgfx::UniformType::Vec4);
    }
}

void AnitoMaterial::setBaseColor(float r, float g, float b, float a) {
    m_baseColor[0] = r;
    m_baseColor[1] = g;
    m_baseColor[2] = b;
    m_baseColor[3] = a;
}

void AnitoMaterial::setEmissive(float r, float g, float b) {
    m_emissive[0] = r;
    m_emissive[1] = g;
    m_emissive[2] = b;
}

void AnitoMaterial::setTexture(TextureSlot slot, std::shared_ptr<AnitoTexture> texture) {
    int index = static_cast<int>(slot);
    if (index >= 0 && index < static_cast<int>(TextureSlot::Count)) {
        m_textures[index] = texture;
    }
}

std::shared_ptr<AnitoTexture> AnitoMaterial::getTexture(TextureSlot slot) const {
    int index = static_cast<int>(slot);
    if (index >= 0 && index < static_cast<int>(TextureSlot::Count)) {
        return m_textures[index];
    }
    return nullptr;
}

void AnitoMaterial::bind(bgfx::ViewId viewId) {
    // Set PBR uniforms
    if (bgfx::isValid(m_uniformBaseColor)) {
        bgfx::setUniform(m_uniformBaseColor, m_baseColor);
    }

    float pbrParams[4] = {
        m_metallic,
        m_roughness,
        m_ao,
        0.0f  // Padding
    };
    if (bgfx::isValid(m_uniformPbrParams)) {
        bgfx::setUniform(m_uniformPbrParams, pbrParams);
    }

    float emissive[4] = {
        m_emissive[0],
        m_emissive[1],
        m_emissive[2],
        0.0f  // Padding
    };
    if (bgfx::isValid(m_uniformEmissive)) {
        bgfx::setUniform(m_uniformEmissive, emissive);
    }

    // Bind textures
    // TODO: Create uniform samplers for each texture slot and bind them
    // For now, we'll use default white texture

    // Note: IBL textures are bound separately by AnitoRenderer::bindIBLTextures()
    // in the mesh renderer before submit
}

std::shared_ptr<AnitoMaterial> AnitoMaterial::createDefault() {
    auto material = std::make_shared<AnitoMaterial>("Default");
    material->setBaseColor(0.8f, 0.8f, 0.8f, 1.0f);
    material->setMetallic(0.0f);
    material->setRoughness(0.5f);
    return material;
}

std::shared_ptr<AnitoMaterial> AnitoMaterial::createPBR(float metallic, float roughness) {
    auto material = std::make_shared<AnitoMaterial>("PBR");
    material->setBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
    material->setMetallic(metallic);
    material->setRoughness(roughness);
    return material;
}

} // namespace Anito
