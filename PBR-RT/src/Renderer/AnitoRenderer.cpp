#include "AnitoRenderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <bx/bx.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb/stb_image.h"

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

void AnitoRenderer::loadEnvironmentMap(const std::string& hdrPath) {
    std::cout << "[AnitoRenderer] Loading environment map: " << hdrPath << std::endl;

    // Load HDR image using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    float* hdrData = stbi_loadf(hdrPath.c_str(), &width, &height, &channels, 4); // Force RGBA

    if (!hdrData) {
        std::cerr << "[AnitoRenderer] Failed to load HDR image: " << hdrPath << std::endl;
        std::cerr << "[AnitoRenderer] stbi error: " << stbi_failure_reason() << std::endl;
        return;
    }

    std::cout << "[AnitoRenderer] Loaded HDR: " << width << "x" << height << " (" << channels << " channels)" << std::endl;

    // Create equirectangular texture
    const bgfx::Memory* equirectMem = bgfx::copy(hdrData, width * height * 4 * sizeof(float));
    bgfx::TextureHandle equirectTexture = bgfx::createTexture2D(
        width, height, false, 1,
        bgfx::TextureFormat::RGBA32F,
        BGFX_TEXTURE_NONE,
        equirectMem
    );

    stbi_image_free(hdrData);

    if (!bgfx::isValid(equirectTexture)) {
        std::cerr << "[AnitoRenderer] Failed to create equirectangular texture!" << std::endl;
        return;
    }

    std::cout << "[AnitoRenderer] Converting equirectangular to cubemap (1024x1024)..." << std::endl;

    // Create cubemap texture (render targets for each face)
    const uint32_t cubemapSize = 1024;
    m_envCubemap = bgfx::createTextureCube(
        cubemapSize,
        false,
        1,
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT
    );

    if (!bgfx::isValid(m_envCubemap)) {
        std::cerr << "[AnitoRenderer] Failed to create cubemap texture!" << std::endl;
        bgfx::destroy(equirectTexture);
        return;
    }

    // Load equirect-to-cubemap conversion shader
    bgfx::ProgramHandle conversionProgram = createProgram(
        "assets/shaders/compiled/vs_equirect_to_cubemap.bin",
        "assets/shaders/compiled/fs_equirect_to_cubemap.bin"
    );

    if (!bgfx::isValid(conversionProgram)) {
        std::cerr << "[AnitoRenderer] Failed to load equirect-to-cubemap shaders!" << std::endl;
        std::cerr << "[AnitoRenderer] Note: Shaders need to be compiled. Will use placeholder." << std::endl;
        bgfx::destroy(equirectTexture);
        // Keep the cubemap but it will be empty
        return;
    }

    // Create uniforms
    bgfx::UniformHandle equirectSampler = bgfx::createUniform("s_texEquirect", bgfx::UniformType::Sampler);
    bgfx::UniformHandle faceParamsUniform = bgfx::createUniform("u_faceParams", bgfx::UniformType::Vec4);

    // Create fullscreen quad vertex and index buffers
    struct PosTexVertex {
        float x, y, z;
        float u, v;
    };

    PosTexVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 0.0f, 1.0f}
    };

    uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)),
        layout
    );

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))
    );

    // Render to each cubemap face
    for (uint8_t face = 0; face < 6; ++face) {
        bgfx::Attachment attachment;
        attachment.init(m_envCubemap, bgfx::Access::Write, face, 1, 0, BGFX_RESOLVE_NONE);

        bgfx::FrameBufferHandle faceFramebuffer = bgfx::createFrameBuffer(1, &attachment, false);

        if (!bgfx::isValid(faceFramebuffer)) {
            std::cerr << "[AnitoRenderer] Failed to create framebuffer for face " << (int)face << std::endl;
            continue;
        }

        // Set view for this face
        bgfx::ViewId conversionView = 10 + face; // Use views 10-15 for conversion
        bgfx::setViewFrameBuffer(conversionView, faceFramebuffer);
        bgfx::setViewRect(conversionView, 0, 0, cubemapSize, cubemapSize);
        bgfx::setViewClear(conversionView, BGFX_CLEAR_COLOR, 0x000000ff, 1.0f, 0);

        // Set face index uniform
        float faceParams[4] = { (float)face, 0.0f, 0.0f, 0.0f };
        bgfx::setUniform(faceParamsUniform, faceParams);

        // Bind equirectangular texture
        bgfx::setTexture(0, equirectSampler, equirectTexture);

        // Set vertex and index buffers
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        // Set state
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

        // Submit draw call
        bgfx::submit(conversionView, conversionProgram);

        // Cleanup framebuffer (attachment keeps cubemap texture)
        bgfx::destroy(faceFramebuffer);
    }

    // Frame to execute all conversions
    bgfx::frame();

    // Cleanup resources
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(equirectSampler);
    bgfx::destroy(faceParamsUniform);
    bgfx::destroy(conversionProgram);
    bgfx::destroy(equirectTexture);

    std::cout << "[AnitoRenderer] Cubemap conversion complete!" << std::endl;

    // Load skybox shaders
    if (!bgfx::isValid(m_skyboxProgram)) {
        std::cout << "[AnitoRenderer] Loading skybox shaders..." << std::endl;
        m_skyboxProgram = createProgram("assets/shaders/compiled/vs_skybox.bin", "assets/shaders/compiled/fs_skybox.bin");
        if (bgfx::isValid(m_skyboxProgram)) {
            m_skyboxCubeUniform = bgfx::createUniform("s_skybox", bgfx::UniformType::Sampler);
            std::cout << "[AnitoRenderer] Skybox shaders loaded successfully" << std::endl;
        } else {
            std::cout << "[AnitoRenderer] Warning: Skybox shaders not found (will be compiled on next build)" << std::endl;
        }
    }

    std::cout << "[AnitoRenderer] Environment map ready for IBL!" << std::endl;

    // Generate irradiance and prefilter maps
    std::cout << "[AnitoRenderer] Generating irradiance map for diffuse IBL..." << std::endl;
    generateIrradianceMap(m_envCubemap);

    std::cout << "[AnitoRenderer] Generating prefiltered environment map for specular IBL..." << std::endl;
    generatePrefilterMap(m_envCubemap);
}

void AnitoRenderer::renderSkybox(const float* viewProjInv, const float* cameraPos) {
    if (!bgfx::isValid(m_envCubemap) || !bgfx::isValid(m_skyboxProgram)) {
        return;
    }

    // Create fullscreen triangle vertices (more efficient than quad)
    struct PosVertex {
        float x, y;
    };

    // Fullscreen triangle covering [-1, 1] NDC space
    static PosVertex skyboxVertices[] = {
        {-1.0f, -1.0f},
        { 3.0f, -1.0f},
        {-1.0f,  3.0f}
    };

    static bgfx::VertexLayout skyboxLayout;
    static bool layoutInit = false;
    if (!layoutInit) {
        skyboxLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .end();
        layoutInit = true;
    }

    // Create transient vertex buffer for skybox
    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, 3, skyboxLayout);

    if (tvb.data) {
        memcpy(tvb.data, skyboxVertices, sizeof(skyboxVertices));

        // Set uniforms
        bgfx::UniformHandle viewProjInvUniform = bgfx::createUniform("u_viewProjInv", bgfx::UniformType::Mat4);
        bgfx::UniformHandle cameraPosUniform = bgfx::createUniform("u_cameraPos", bgfx::UniformType::Vec4);

        bgfx::setUniform(viewProjInvUniform, viewProjInv);
        bgfx::setUniform(cameraPosUniform, cameraPos);

        // Bind cubemap
        bgfx::setTexture(0, m_skyboxCubeUniform, m_envCubemap);

        // Set vertex buffer
        bgfx::setVertexBuffer(0, &tvb);

        // Render state: write to color only, depth = 1.0 (furthest)
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_LEQUAL);

        // Submit to a background view (render before main geometry)
        bgfx::ViewId skyboxView = 0; // Use view 0 for skybox background
        bgfx::submit(skyboxView, m_skyboxProgram);

        // Cleanup transient uniforms (note: don't destroy if they're reused)
        // For now, we'll recreate each frame - for production, cache these
        bgfx::destroy(viewProjInvUniform);
        bgfx::destroy(cameraPosUniform);
    }
}

void AnitoRenderer::generateIrradianceMap(bgfx::TextureHandle envCubemap) {
    // Create irradiance cubemap (smaller resolution: 32x32 is sufficient for diffuse)
    const uint32_t irradianceSize = 32;
    m_irradianceMap = bgfx::createTextureCube(
        irradianceSize,
        false,
        1,
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT
    );

    if (!bgfx::isValid(m_irradianceMap)) {
        std::cerr << "[AnitoRenderer] Failed to create irradiance cubemap!" << std::endl;
        return;
    }

    // Load irradiance convolution shader
    bgfx::ProgramHandle convolutionProgram = createProgram(
        "assets/shaders/vs_irradiance_convolution.bin",
        "assets/shaders/fs_irradiance_convolution.bin"
    );

    if (!bgfx::isValid(convolutionProgram)) {
        std::cerr << "[AnitoRenderer] Failed to load irradiance convolution shaders!" << std::endl;
        bgfx::destroy(m_irradianceMap);
        m_irradianceMap = BGFX_INVALID_HANDLE;
        return;
    }

    // Create uniforms
    bgfx::UniformHandle envMapSampler = bgfx::createUniform("s_envMap", bgfx::UniformType::Sampler);
    bgfx::UniformHandle faceParamsUniform = bgfx::createUniform("u_faceParams", bgfx::UniformType::Vec4);

    // Create fullscreen quad
    struct PosTexVertex {
        float x, y, z;
        float u, v;
    };

    PosTexVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 0.0f, 1.0f}
    };

    uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)),
        layout
    );

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))
    );

    // Render to each cubemap face
    for (uint8_t face = 0; face < 6; ++face) {
        bgfx::Attachment attachment;
        attachment.init(m_irradianceMap, bgfx::Access::Write, 0, 0, face, BGFX_RESOLVE_NONE);

        bgfx::FrameBufferHandle faceFramebuffer = bgfx::createFrameBuffer(1, &attachment, false);

        if (!bgfx::isValid(faceFramebuffer)) {
            std::cerr << "[AnitoRenderer] Failed to create irradiance framebuffer for face " << (int)face << std::endl;
            continue;
        }

        // Set view for this face
        bgfx::ViewId convolutionView = 16 + face; // Use views 16-21 for irradiance
        bgfx::setViewFrameBuffer(convolutionView, faceFramebuffer);
        bgfx::setViewRect(convolutionView, 0, 0, irradianceSize, irradianceSize);
        bgfx::setViewClear(convolutionView, BGFX_CLEAR_COLOR, 0x000000ff, 1.0f, 0);

        // Set face index uniform
        float faceParams[4] = { (float)face, 0.0f, 0.0f, 0.0f };
        bgfx::setUniform(faceParamsUniform, faceParams);

        // Bind environment cubemap
        bgfx::setTexture(0, envMapSampler, envCubemap);

        // Set vertex and index buffers
        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        // Set state
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

        // Submit draw call
        bgfx::submit(convolutionView, convolutionProgram);

        // Cleanup framebuffer
        bgfx::destroy(faceFramebuffer);
    }

    // Frame to execute all convolutions
    bgfx::frame();

    // Cleanup resources
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(envMapSampler);
    bgfx::destroy(faceParamsUniform);
    bgfx::destroy(convolutionProgram);

    std::cout << "[AnitoRenderer] Irradiance map generation complete!" << std::endl;
}

void AnitoRenderer::generatePrefilterMap(bgfx::TextureHandle envCubemap) {
    // Create prefilter cubemap with mipmaps (512x512 base, 5 mip levels for roughness)
    const uint32_t prefilterSize = 512;
    const uint32_t numMips = 5; // Mip 0-4 for roughness 0.0-1.0

    m_prefilterMap = bgfx::createTextureCube(
        prefilterSize,
        true,  // hasMips
        numMips,
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT
    );

    if (!bgfx::isValid(m_prefilterMap)) {
        std::cerr << "[AnitoRenderer] Failed to create prefilter cubemap!" << std::endl;
        return;
    }

    // Load prefilter shader
    bgfx::ProgramHandle prefilterProgram = createProgram(
        "assets/shaders/vs_prefilter_envmap.bin",
        "assets/shaders/fs_prefilter_envmap.bin"
    );

    if (!bgfx::isValid(prefilterProgram)) {
        std::cerr << "[AnitoRenderer] Failed to load prefilter shaders!" << std::endl;
        bgfx::destroy(m_prefilterMap);
        m_prefilterMap = BGFX_INVALID_HANDLE;
        return;
    }

    // Create uniforms
    bgfx::UniformHandle envMapSampler = bgfx::createUniform("s_envMap", bgfx::UniformType::Sampler);
    bgfx::UniformHandle faceParamsUniform = bgfx::createUniform("u_faceParams", bgfx::UniformType::Vec4);
    bgfx::UniformHandle sampleCountUniform = bgfx::createUniform("u_sampleCount", bgfx::UniformType::Vec4);

    // Create fullscreen quad
    struct PosTexVertex {
        float x, y, z;
        float u, v;
    };

    PosTexVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 0.0f, 1.0f}
    };

    uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(vertices)),
        layout
    );

    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(
        bgfx::makeRef(indices, sizeof(indices))
    );

    // Sample count for importance sampling
    float sampleCount[4] = { 1024.0f, 0.0f, 0.0f, 0.0f };
    bgfx::setUniform(sampleCountUniform, sampleCount);

    // Render to each mip level (roughness variation)
    for (uint8_t mip = 0; mip < numMips; ++mip) {
        uint32_t mipSize = prefilterSize >> mip; // 512, 256, 128, 64, 32
        float roughness = static_cast<float>(mip) / static_cast<float>(numMips - 1); // 0.0, 0.25, 0.5, 0.75, 1.0

        // Render to each cubemap face
        for (uint8_t face = 0; face < 6; ++face) {
            bgfx::Attachment attachment;
            attachment.init(m_prefilterMap, bgfx::Access::Write, 0, mip, face, BGFX_RESOLVE_NONE);

            bgfx::FrameBufferHandle faceFramebuffer = bgfx::createFrameBuffer(1, &attachment, false);

            if (!bgfx::isValid(faceFramebuffer)) {
                std::cerr << "[AnitoRenderer] Failed to create prefilter framebuffer for face " << (int)face << ", mip " << (int)mip << std::endl;
                continue;
            }

            // Set view for this face+mip
            bgfx::ViewId prefilterView = 22 + mip * 6 + face; // Views 22-51 for prefiltering
            bgfx::setViewFrameBuffer(prefilterView, faceFramebuffer);
            bgfx::setViewRect(prefilterView, 0, 0, mipSize, mipSize);
            bgfx::setViewClear(prefilterView, BGFX_CLEAR_COLOR, 0x000000ff, 1.0f, 0);

            // Set face/roughness parameters
            float faceParams[4] = { (float)face, roughness, (float)mipSize, 0.0f };
            bgfx::setUniform(faceParamsUniform, faceParams);

            // Bind environment cubemap
            bgfx::setTexture(0, envMapSampler, envCubemap);

            // Set vertex and index buffers
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);

            // Set state
            bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);

            // Submit draw call
            bgfx::submit(prefilterView, prefilterProgram);

            // Cleanup framebuffer
            bgfx::destroy(faceFramebuffer);
        }
    }

    // Frame to execute all prefiltering
    bgfx::frame();

    // Cleanup resources
    bgfx::destroy(vbh);
    bgfx::destroy(ibh);
    bgfx::destroy(envMapSampler);
    bgfx::destroy(faceParamsUniform);
    bgfx::destroy(sampleCountUniform);
    bgfx::destroy(prefilterProgram);

    std::cout << "[AnitoRenderer] Prefilter map generation complete!" << std::endl;
}

} // namespace Anito
