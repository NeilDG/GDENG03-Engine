#include "AnitoEngine.h"
#include "Window/AnitoWindow.h"
#include "Renderer/AnitoRenderer.h"
#include "Renderer/AnitoVertexBuffer.h"
#include "Renderer/AnitoShader.h"
#include "Renderer/AnitoMaterial.h"
#include "Renderer/AnitoMeshGenerator.h"
#include "Renderer/AnitoPBRTestScenes.h"
#include "GameObjects/AnitoGameObjectManager.h"
#include "GameObjects/AnitoGameObject.h"
#include "Components/AnitoMeshRenderer.h"
#include "Components/AnitoTransform.h"
#include "Components/AnitoCamera.h"
#include "Input/AnitoInputManager.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Anito {

AnitoEngine* AnitoEngine::s_instance = nullptr;

AnitoEngine::AnitoEngine()
    : m_window(nullptr)
    , m_running(false)
    , m_lastFrameTime(0.0f)
    , m_pbrTestScenes(nullptr)
    , m_currentCameraPos{0.0f, 5.0f, 15.0f}
{
    s_instance = this;
}

AnitoEngine::~AnitoEngine() {
    shutdown();
    s_instance = nullptr;
}

bool AnitoEngine::initialize(const std::string& title, uint32_t width, uint32_t height) {
    std::cout << "==================================================" << std::endl;
    std::cout << "  Anito Engine - Project Anito (DOST-PCIEERD)" << std::endl;
    std::cout << "  DLSU GAME Lab - AAA Game Engine Development" << std::endl;
    std::cout << "  Principal Investigator: Neil Patrick Del Gallego, Ph.D." << std::endl;
    std::cout << "==================================================" << std::endl;

    // Create window
    m_window = AnitoWindow::create(title, width, height);
    if (!m_window) {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    // Initialize renderer
    void* nativeHandle = m_window->getNativeWindowHandle();
    AnitoRenderer::initialize(nativeHandle, width, height);

    // Set up window resize callback
    m_window->setResizeCallback([](uint32_t w, uint32_t h) {
        if (AnitoRenderer::getInstance()) {
            AnitoRenderer::getInstance()->resize(w, h);
        }
    });

    // Initialize input manager
    AnitoInputManager::initialize(m_window->getGLFWWindow());
    std::cout << "[Engine] Input Manager initialized" << std::endl;

    // Initialize vertex layouts
    PosColorVertex::init();
    PosNormalTexcoordVertex::init();

    // Initialize GameObjectManager
    AnitoGameObjectManager::initialize();

    // Create test scene
    createTestScene();

    m_running = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    std::cout << "Anito Engine initialized successfully!" << std::endl;
    std::cout << "==================================================" << std::endl;
    return true;
}

void AnitoEngine::createTestScene() {
    std::cout << "\n[Scene Setup] Creating PBR test scenes..." << std::endl;

    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    // Load HDR environment map for IBL
    std::cout << "[Scene Setup] Loading HDR environment map..." << std::endl;
    renderer->loadEnvironmentMap("assets/hdr/relax_inn_seaview_suite_4k.hdr");

    // Load shader
    std::cout << "[Scene Setup] Loading shader..." << std::endl;
    auto shader = AnitoShader::createOrGet("SimpleShader", 
        "assets/shaders/compiled/vs_simple.bin",
        "assets/shaders/compiled/fs_simple.bin");

    if (!shader || !shader->isValid()) {
        std::cerr << "[Scene Setup] WARNING: Failed to load shader!" << std::endl;
        std::cerr << "[Scene Setup] Shaders need to be compiled first." << std::endl;
        std::cerr << "[Scene Setup] TO COMPILE SHADERS:" << std::endl;
        std::cerr << "[Scene Setup]   1. Download bgfx prebuilt tools from:" << std::endl;
        std::cerr << "[Scene Setup]      https://github.com/bkaradzic/bgfx/releases" << std::endl;
        std::cerr << "[Scene Setup]   2. Extract shaderc.exe to: external/bgfx/.build/win64_vs2022/bin/" << std::endl;
        std::cerr << "[Scene Setup]   3. Run: powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1" << std::endl;
        std::cerr << "[Scene Setup] " << std::endl;
        std::cerr << "[Scene Setup] For now, the engine will run but not render anything." << std::endl;
        std::cout << "==================================================" << std::endl;
        return;
    }

    // Create uniforms for lighting and IBL
    shader->createUniform("u_lightDir", bgfx::UniformType::Vec4);
    shader->createUniform("u_baseColor", bgfx::UniformType::Vec4);
    shader->createUniform("u_pbrParams", bgfx::UniformType::Vec4);
    shader->createUniform("u_cameraPos", bgfx::UniformType::Vec4);  // Camera position uniform
    shader->createUniform("u_envMap", bgfx::UniformType::Sampler);  // Environment cubemap sampler
    shader->createUniform("u_irradianceMap", bgfx::UniformType::Sampler);  // Irradiance map for diffuse IBL
    shader->createUniform("u_prefilterMap", bgfx::UniformType::Sampler);   // Prefiltered map for specular IBL
    shader->createUniform("u_enableIBL", bgfx::UniformType::Vec4);   // IBL toggle (as vec4 for bgfx compatibility)

    // Initialize PBR test scenes system
    std::cout << "[Scene Setup] Initializing PBR test scenes..." << std::endl;
    m_pbrTestScenes = std::make_unique<AnitoPBRTestScenes>();
    m_pbrTestScenes->initialize(shader);

    // Setup camera view for the first scene
    std::cout << "[Scene Setup] Setting up camera..." << std::endl;
    glm::vec3 cameraPos = m_pbrTestScenes->getCameraPositionForScene();
    glm::vec3 lookAtPos = m_pbrTestScenes->getLookAtPositionForScene();

    // Store camera position for per-frame uniform updates
    m_currentCameraPos[0] = cameraPos.x;
    m_currentCameraPos[1] = cameraPos.y;
    m_currentCameraPos[2] = cameraPos.z;

    AnitoMatrix4x4 view = AnitoMatrix4x4::lookAt(
        cameraPos,
        lookAtPos,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
    AnitoMatrix4x4 proj = AnitoMatrix4x4::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    bgfx::setViewTransform(renderer->getMainViewId(), view.data(), proj.data());

    // Set light direction (directional light from above-front-right)
    // Direction points FROM light source, intensity increased for PBR test visibility
    float lightDir[4] = { -0.3f, -0.5f, 0.8f, 12.0f }; // w = intensity
    shader->setUniform("u_lightDir", lightDir);

    std::cout << "[Scene Setup] PBR test scenes created successfully!" << std::endl;
    std::cout << "  - Current scene: " << m_pbrTestScenes->getCurrentSceneConfig().name << std::endl;
    std::cout << "  - Description: " << m_pbrTestScenes->getCurrentSceneConfig().description << std::endl;
    std::cout << "  - Press SPACE to switch between test scenes" << std::endl;
    std::cout << "  - Press Z to toggle IBL (Image-Based Lighting)" << std::endl;
    std::cout << "==================================================" << std::endl;
}

void AnitoEngine::run() {
    std::cout << "\n[Engine] Starting main loop..." << std::endl;

    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // Process events
        m_window->pollEvents();

        // Update
        update(deltaTime);

        // Render
        render();
    }

    std::cout << "[Engine] Main loop ended." << std::endl;
}

void AnitoEngine::shutdown() {
    std::cout << "\n[Engine] Shutting down Anito Engine..." << std::endl;

    // Destroy game objects
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::destroy();
    }

    // Clear shader and texture caches
    AnitoShader::clearCache();
    AnitoTexture::clearCache();

    // Destroy subsystems in reverse order
    AnitoInputManager::destroy();
    AnitoRenderer::destroy();

    if (m_window) {
        delete m_window;
        m_window = nullptr;
    }

    m_running = false;
    std::cout << "[Engine] Shutdown complete." << std::endl;
}

void AnitoEngine::update(float deltaTime) {
    // Update input
    if (AnitoInputManager::getInstance()) {
        AnitoInputManager::getInstance()->update();

        // Handle scene switching with spacebar
        if (m_pbrTestScenes && AnitoInputManager::getInstance()->isKeyPressed(GLFW_KEY_SPACE)) {
            m_pbrTestScenes->switchToNextScene();

            // Update camera for new scene
            AnitoRenderer* renderer = AnitoRenderer::getInstance();
            if (renderer) {
                glm::vec3 cameraPos = m_pbrTestScenes->getCameraPositionForScene();
                glm::vec3 lookAtPos = m_pbrTestScenes->getLookAtPositionForScene();

                // Store updated camera position
                m_currentCameraPos[0] = cameraPos.x;
                m_currentCameraPos[1] = cameraPos.y;
                m_currentCameraPos[2] = cameraPos.z;

                AnitoMatrix4x4 view = AnitoMatrix4x4::lookAt(
                    cameraPos,
                    lookAtPos,
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );

                float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
                AnitoMatrix4x4 proj = AnitoMatrix4x4::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

                bgfx::setViewTransform(renderer->getMainViewId(), view.data(), proj.data());
            }
        }
    }

    // Update PBR test scenes (rotating cubes, etc.)
    if (m_pbrTestScenes) {
        m_pbrTestScenes->update(deltaTime);
    }

    // Update all game objects
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->updateAll(deltaTime);
    }
}

void AnitoEngine::render() {
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) return;

    renderer->beginFrame();

    // STEP 1: Render skybox first (background)
    if (renderer->isIBLReady() && renderer->getEnableIBL()) {
        // Calculate view-projection inverse matrix for skybox
        glm::vec3 cameraPos(m_currentCameraPos[0], m_currentCameraPos[1], m_currentCameraPos[2]);
        glm::vec3 lookAtPos(0.0f, 0.0f, 0.0f);
        AnitoMatrix4x4 view = AnitoMatrix4x4::lookAt(cameraPos, lookAtPos, glm::vec3(0.0f, 1.0f, 0.0f));

        float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
        AnitoMatrix4x4 proj = AnitoMatrix4x4::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

        // Calculate view-projection matrix
        glm::mat4 viewProj = glm::make_mat4(proj.data()) * glm::make_mat4(view.data());
        glm::mat4 viewProjInv = glm::inverse(viewProj);

        // Render skybox
        renderer->renderSkybox(glm::value_ptr(viewProjInv), m_currentCameraPos);
    }

    // Set global lighting uniforms (must be set every frame for bgfx)
    if (m_pbrTestScenes) {
        auto shader = AnitoShader::get("SimpleShader");
        if (shader && shader->isValid()) {
            // Set light direction (directional light from above-front-right)
            // Direction points FROM light source, intensity increased for PBR test visibility
            float lightDir[4] = { -0.3f, -0.5f, 0.8f, 12.0f }; // w = intensity
            shader->setUniform("u_lightDir", lightDir);

            // Set camera position
            float cameraPos[4] = { m_currentCameraPos[0], m_currentCameraPos[1], m_currentCameraPos[2], 1.0f };
            shader->setUniform("u_cameraPos", cameraPos);

            // Set IBL environment cubemap and toggle
            if (renderer->isIBLReady()) {
                bgfx::TextureHandle envCubemap = renderer->getEnvCubemap();
                bgfx::TextureHandle irradianceMap = renderer->getIrradianceMap();
                bgfx::TextureHandle prefilterMap = renderer->getPrefilterMap();

                // Bind environment cubemap (for skybox/fallback)
                bgfx::UniformHandle envMapUniform = shader->getUniformHandle("u_envMap");
                if (bgfx::isValid(envMapUniform) && bgfx::isValid(envCubemap)) {
                    bgfx::setTexture(0, envMapUniform, envCubemap);
                }

                // Bind irradiance map (for diffuse IBL)
                bgfx::UniformHandle irradianceMapUniform = shader->getUniformHandle("u_irradianceMap");
                if (bgfx::isValid(irradianceMapUniform) && bgfx::isValid(irradianceMap)) {
                    bgfx::setTexture(1, irradianceMapUniform, irradianceMap);
                }

                // Bind prefilter map (for specular IBL with roughness)
                bgfx::UniformHandle prefilterMapUniform = shader->getUniformHandle("u_prefilterMap");
                if (bgfx::isValid(prefilterMapUniform) && bgfx::isValid(prefilterMap)) {
                    bgfx::setTexture(2, prefilterMapUniform, prefilterMap);
                }

                // Set IBL toggle state (pass as vec4 for bgfx compatibility)
                float iblEnabled[4] = { renderer->getEnableIBL() ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f };
                bgfx::UniformHandle iblToggleUniform = shader->getUniformHandle("u_enableIBL");
                if (bgfx::isValid(iblToggleUniform)) {
                    bgfx::setUniform(iblToggleUniform, iblEnabled);
                }
            }
        }
    }

    // STEP 2: Render all game objects (foreground)
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->renderAll();
    }

    renderer->endFrame();
}

} // namespace Anito
