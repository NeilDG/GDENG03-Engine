#include "AnitoShader.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace Anito {

// Initialize static cache
std::unordered_map<std::string, std::shared_ptr<AnitoShader>> AnitoShader::s_shaderCache;

AnitoShader::AnitoShader(const std::string& name)
    : m_name(name)
    , m_program(BGFX_INVALID_HANDLE)
{
}

AnitoShader::~AnitoShader() {
    // Destroy all cached uniforms
    for (auto& pair : m_uniforms) {
        if (bgfx::isValid(pair.second)) {
            bgfx::destroy(pair.second);
        }
    }
    m_uniforms.clear();

    // Destroy program
    if (bgfx::isValid(m_program)) {
        bgfx::destroy(m_program);
    }
}

bool AnitoShader::loadFromFiles(const std::string& vsPath, const std::string& fsPath) {
    // Load vertex shader
    std::ifstream vsFile(vsPath, std::ios::binary | std::ios::ate);
    if (!vsFile.is_open()) {
        std::cerr << "[AnitoShader] Failed to open vertex shader: " << vsPath << std::endl;
        return false;
    }

    std::streamsize vsSize = vsFile.tellg();
    vsFile.seekg(0, std::ios::beg);
    std::vector<char> vsBuffer(vsSize);
    if (!vsFile.read(vsBuffer.data(), vsSize)) {
        std::cerr << "[AnitoShader] Failed to read vertex shader: " << vsPath << std::endl;
        return false;
    }

    // Load fragment shader
    std::ifstream fsFile(fsPath, std::ios::binary | std::ios::ate);
    if (!fsFile.is_open()) {
        std::cerr << "[AnitoShader] Failed to open fragment shader: " << fsPath << std::endl;
        return false;
    }

    std::streamsize fsSize = fsFile.tellg();
    fsFile.seekg(0, std::ios::beg);
    std::vector<char> fsBuffer(fsSize);
    if (!fsFile.read(fsBuffer.data(), fsSize)) {
        std::cerr << "[AnitoShader] Failed to read fragment shader: " << fsPath << std::endl;
        return false;
    }

    return loadFromMemory(vsBuffer.data(), static_cast<uint32_t>(vsSize), 
                          fsBuffer.data(), static_cast<uint32_t>(fsSize));
}

bool AnitoShader::loadFromMemory(const void* vsData, uint32_t vsSize, const void* fsData, uint32_t fsSize) {
    // Create vertex shader
    const bgfx::Memory* vsMem = bgfx::copy(vsData, vsSize);
    bgfx::ShaderHandle vsh = bgfx::createShader(vsMem);

    if (!bgfx::isValid(vsh)) {
        std::cerr << "[AnitoShader] Failed to create vertex shader for: " << m_name << std::endl;
        return false;
    }

    // Create fragment shader
    const bgfx::Memory* fsMem = bgfx::copy(fsData, fsSize);
    bgfx::ShaderHandle fsh = bgfx::createShader(fsMem);

    if (!bgfx::isValid(fsh)) {
        std::cerr << "[AnitoShader] Failed to create fragment shader for: " << m_name << std::endl;
        bgfx::destroy(vsh);
        return false;
    }

    // Create program (bgfx takes ownership of shaders when destroyShaders is true)
    m_program = bgfx::createProgram(vsh, fsh, true);

    if (!bgfx::isValid(m_program)) {
        std::cerr << "[AnitoShader] Failed to link shader program: " << m_name << std::endl;
        return false;
    }

    std::cout << "[AnitoShader] Successfully loaded shader: " << m_name << std::endl;
    return true;
}

void AnitoShader::setUniform(const std::string& name, const void* value, uint16_t num) {
    bgfx::UniformHandle uniform = getUniform(name);
    if (bgfx::isValid(uniform)) {
        bgfx::setUniform(uniform, value, num);
    }
}

void AnitoShader::setTexture(const std::string& name, bgfx::TextureHandle texture, uint8_t stage) {
    bgfx::UniformHandle uniform = getUniform(name);
    if (bgfx::isValid(uniform)) {
        bgfx::setTexture(stage, uniform, texture);
    }
}

bgfx::UniformHandle AnitoShader::getUniform(const std::string& name) {
    auto it = m_uniforms.find(name);
    if (it != m_uniforms.end()) {
        return it->second;
    }
    return BGFX_INVALID_HANDLE;
}

bgfx::UniformHandle AnitoShader::createUniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t num) {
    // Check if uniform already exists
    auto it = m_uniforms.find(name);
    if (it != m_uniforms.end()) {
        return it->second;
    }

    // Create new uniform
    bgfx::UniformHandle uniform = bgfx::createUniform(name.c_str(), type, num);
    if (bgfx::isValid(uniform)) {
        m_uniforms[name] = uniform;
    } else {
        std::cerr << "[AnitoShader] Failed to create uniform: " << name << std::endl;
    }

    return uniform;
}

std::shared_ptr<AnitoShader> AnitoShader::createOrGet(const std::string& name, 
                                                       const std::string& vsPath, 
                                                       const std::string& fsPath) {
    // Check cache
    auto it = s_shaderCache.find(name);
    if (it != s_shaderCache.end()) {
        return it->second;
    }

    // Create new shader
    auto shader = std::make_shared<AnitoShader>(name);
    if (!shader->loadFromFiles(vsPath, fsPath)) {
        return nullptr;
    }

    // Add to cache
    s_shaderCache[name] = shader;
    return shader;
}

void AnitoShader::clearCache() {
    s_shaderCache.clear();
}

} // namespace Anito
