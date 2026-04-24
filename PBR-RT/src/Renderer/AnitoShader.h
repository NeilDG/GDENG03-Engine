#pragma once
#include <bgfx/bgfx.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace Anito {

/**
 * AnitoShader - Shader program management
 * Handles loading, compilation, and uniform management for bgfx shaders
 */
class AnitoShader {
public:
    AnitoShader(const std::string& name);
    ~AnitoShader();

    // Loading and compilation
    bool loadFromFiles(const std::string& vsPath, const std::string& fsPath);
    bool loadFromMemory(const void* vsData, uint32_t vsSize, const void* fsData, uint32_t fsSize);

    // Shader program management
    bool isValid() const { return bgfx::isValid(m_program); }
    bgfx::ProgramHandle getProgram() const { return m_program; }

    // Uniform management
    void setUniform(const std::string& name, const void* value, uint16_t num = 1);
    void setTexture(const std::string& name, bgfx::TextureHandle texture, uint8_t stage = 0);

    bgfx::UniformHandle getUniform(const std::string& name);
    bgfx::UniformHandle createUniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t num = 1);

    // Utility
    const std::string& getName() const { return m_name; }

    // Static shader cache
    static std::shared_ptr<AnitoShader> createOrGet(const std::string& name, 
                                                     const std::string& vsPath, 
                                                     const std::string& fsPath);
    static std::shared_ptr<AnitoShader> get(const std::string& name);
    static void clearCache();

private:
    std::string m_name;
    bgfx::ProgramHandle m_program;

    // Uniform cache
    std::unordered_map<std::string, bgfx::UniformHandle> m_uniforms;

    // Global shader cache
    static std::unordered_map<std::string, std::shared_ptr<AnitoShader>> s_shaderCache;
};

} // namespace Anito
