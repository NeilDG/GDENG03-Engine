#pragma once
#include "AnitoShader.h"
#include "AnitoTexture.h"
#include <memory>
#include <string>

namespace Anito {

/**
 * AnitoMaterial - PBR Material System
 * Stores material parameters and textures for physically-based rendering
 */
class AnitoMaterial {
public:
    // PBR texture slots
    enum class TextureSlot {
        Albedo = 0,
        Normal = 1,
        Metallic = 2,
        Roughness = 3,
        AO = 4,          // Ambient Occlusion
        Emissive = 5,
        Count
    };

public:
    AnitoMaterial(const std::string& name = "Material");
    ~AnitoMaterial();

    // Shader
    void setShader(std::shared_ptr<AnitoShader> shader) { m_shader = shader; }
    std::shared_ptr<AnitoShader> getShader() const { return m_shader; }

    // PBR Parameters
    void setBaseColor(float r, float g, float b, float a = 1.0f);
    void setMetallic(float metallic) { m_metallic = metallic; }
    void setRoughness(float roughness) { m_roughness = roughness; }
    void setAO(float ao) { m_ao = ao; }
    void setEmissive(float r, float g, float b);

    float* getBaseColor() { return m_baseColor; }
    float getMetallic() const { return m_metallic; }
    float getRoughness() const { return m_roughness; }
    float getAO() const { return m_ao; }
    float* getEmissive() { return m_emissive; }

    // Texture management
    void setTexture(TextureSlot slot, std::shared_ptr<AnitoTexture> texture);
    std::shared_ptr<AnitoTexture> getTexture(TextureSlot slot) const;

    // Utility
    void bind(bgfx::ViewId viewId);
    const std::string& getName() const { return m_name; }

    // Static default materials
    static std::shared_ptr<AnitoMaterial> createDefault();
    static std::shared_ptr<AnitoMaterial> createPBR(float metallic = 0.0f, float roughness = 0.5f);

private:
    void setupDefaultUniforms();

    std::string m_name;
    std::shared_ptr<AnitoShader> m_shader;

    // PBR parameters
    float m_baseColor[4];     // RGBA
    float m_metallic;
    float m_roughness;
    float m_ao;
    float m_emissive[3];      // RGB

    // Textures
    std::shared_ptr<AnitoTexture> m_textures[static_cast<int>(TextureSlot::Count)];

    // Uniform handles (created on demand)
    bgfx::UniformHandle m_uniformBaseColor;
    bgfx::UniformHandle m_uniformPbrParams;
    bgfx::UniformHandle m_uniformEmissive;
};

} // namespace Anito
