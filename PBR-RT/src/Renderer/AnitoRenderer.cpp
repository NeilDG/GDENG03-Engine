#include "AnitoRenderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bx/bx.h>

namespace Anito {

AnitoRenderer* AnitoRenderer::s_instance = nullptr;

AnitoRenderer* AnitoRenderer::getInstance() {
    return s_instance;
}

void AnitoRenderer::initialize(void* nativeWindowHandle, uint32_t width, uint32_t height) {
    if (s_instance == nullptr) {
        s_instance = new AnitoRenderer();
        s_instance->init(nativeWindowHandle, width, height);
    }
}

void AnitoRenderer::destroy() {
    if (s_instance != nullptr) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoRenderer::AnitoRenderer()
    : m_width(1280)
    , m_height(720)
    , m_mainViewId(0)
    , m_resetFlags(BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4)
{
}

AnitoRenderer::~AnitoRenderer() {
}

void AnitoRenderer::init(void* nativeWindowHandle, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;

    // Initialize bgfx
    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // Auto-select renderer (Vulkan preferred on Windows)
    init.vendorId = BGFX_PCI_ID_NONE;
    init.platformData.nwh = nativeWindowHandle;
    init.resolution.width = m_width;
    init.resolution.height = m_height;
    init.resolution.reset = m_resetFlags;

    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize bgfx!" << std::endl;
        return;
    }

    // Print renderer info
    const bgfx::Caps* caps = bgfx::getCaps();
    std::cout << "Anito Renderer initialized successfully" << std::endl;
    std::cout << "Renderer: " << bgfx::getRendererName(caps->rendererType) << std::endl;
    std::cout << "Vendor: " << caps->vendorId << std::endl;

    // Configure main view
    bgfx::setViewClear(m_mainViewId,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        0x303030ff, // Dark gray background
        1.0f,
        0);

    bgfx::setViewRect(m_mainViewId, 0, 0, uint16_t(m_width), uint16_t(m_height));
}

void AnitoRenderer::release() {
    bgfx::shutdown();
}

void AnitoRenderer::beginFrame() {
    // Touch view to ensure it's submitted
    bgfx::touch(m_mainViewId);
}

void AnitoRenderer::endFrame() {
    // Advance to next frame
    bgfx::frame();
}

void AnitoRenderer::resize(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    bgfx::reset(m_width, m_height, m_resetFlags);
    bgfx::setViewRect(m_mainViewId, 0, 0, uint16_t(m_width), uint16_t(m_height));
}

bgfx::ShaderHandle AnitoRenderer::createShader(const std::string& shaderPath) {
    std::ifstream file(shaderPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << shaderPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Failed to read shader file: " << shaderPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), uint32_t(size));
    bgfx::ShaderHandle handle = bgfx::createShader(mem);

    if (!bgfx::isValid(handle)) {
        std::cerr << "Failed to create shader from: " << shaderPath << std::endl;
    }

    return handle;
}

bgfx::ProgramHandle AnitoRenderer::createProgram(const std::string& vsPath, const std::string& fsPath) {
    bgfx::ShaderHandle vsh = createShader(vsPath);
    bgfx::ShaderHandle fsh = createShader(fsPath);

    if (!bgfx::isValid(vsh) || !bgfx::isValid(fsh)) {
        std::cerr << "Failed to create program: invalid shaders" << std::endl;
        return BGFX_INVALID_HANDLE;
    }

    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    if (!bgfx::isValid(program)) {
        std::cerr << "Failed to link shader program" << std::endl;
    }

    return program;
}

} // namespace Anito
