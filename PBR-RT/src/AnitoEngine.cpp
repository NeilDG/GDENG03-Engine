#include "AnitoEngine.h"
#include "Config/AnitoEngineConfig.h"
#include "Window/AnitoWindow.h"
#include "Renderer/AnitoRenderer.h"
#include "Renderer/AnitoVertexBuffer.h"
#include "Renderer/AnitoShader.h"
#include "Renderer/AnitoMaterial.h"
#include "Renderer/AnitoMeshGenerator.h"
#include "Renderer/AnitoPBRTestScenes.h"
#include "Renderer/Deferred/AnitoGBuffer.h"
#include "Renderer/Deferred/AnitoDeferredRenderer.h"
#include "DebugTools/AnitoFrameCaptureRecorder.h"
#include "DebugTools/AnitoProfilerManager.h"
#include "GameObjects/AnitoGameObjectManager.h"
#include "GameObjects/AnitoGameObject.h"
#include "Components/AnitoMeshRenderer.h"
#include "Components/AnitoTransform.h"
#include "Components/AnitoCamera.h"
#include "Components/AnitoFPSCameraControl.h"
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
    , m_maxRuntimeSeconds(0.0f)
    , m_elapsedRuntime(0.0f)
    , m_pbrTestScenes(nullptr)
    , m_frameCaptureRecorder(nullptr)
    , m_cameraObject(nullptr)
    , m_camera(nullptr)
    , m_useDeferredRendering(false)  // Start with forward rendering (Step 7)
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

    // Load configuration file
    std::cout << "[Engine] Loading configuration..." << std::endl;
    AnitoEngineConfig::load("engine_config.ini");

    // Get runtime duration setting
    m_maxRuntimeSeconds = AnitoEngineConfig::getFloat("Runtime.MaxRuntimeSeconds", 0.0f);
    if (m_maxRuntimeSeconds > 0.0f) {
        std::cout << "[Engine] Runtime limit: " << m_maxRuntimeSeconds << " seconds" << std::endl;
        std::cout << "[Engine] Engine will automatically exit after this duration." << std::endl;
    } else {
        std::cout << "[Engine] Runtime limit: DISABLED (will run indefinitely)" << std::endl;
    }

    // Initialize profiling system
    std::cout << "[Engine] Initializing profiling system..." << std::endl;
    AnitoProfilerManager::initialize(
        "anito-debug",
        true,  // Enable CPU profiling
        true,  // Enable GPU profiling
        AnitoMemoryProfiler::TrackingMode::LIGHT,  // Use light memory tracking
        true   // Enable crash reporter
    );
    std::cout << "[Engine] Profiling system initialized - Output: anito-debug/" << std::endl;

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

    // Initialize frame capture recorder
    m_frameCaptureRecorder = std::make_unique<AnitoFrameCaptureRecorder>();
    m_frameCaptureRecorder->initialize("anito-debug/frames", 1.0f, "png");  // Capture every 1 second

    // [STEP 1-7] Initialize deferred rendering system
    std::cout << "\n[Engine] === PHASE 3: DEFERRED RENDERING INITIALIZATION ===" << std::endl;
    std::cout << "[Engine] Step 1-3: G-Buffer creation..." << std::endl;
    m_deferredRenderer = std::make_unique<AnitoDeferredRenderer>(width, height);
    std::cout << "[Engine] Step 4-6: G-Buffer shaders compiled (verified earlier)" << std::endl;
    std::cout << "[Engine] Step 7: Loading G-Buffer shaders for geometry pass..." << std::endl;

    // Load G-Buffer shaders (Step 7)
    m_gbufferShader = AnitoShader::createOrGet("GBufferShader",
        "assets/shaders/compiled/vs_gbuffer.bin",
        "assets/shaders/compiled/fs_gbuffer.bin");

    if (!m_gbufferShader || !m_gbufferShader->isValid()) {
        std::cerr << "[Engine] ERROR: Failed to load G-Buffer shaders!" << std::endl;
        std::cerr << "[Engine] Deferred rendering will be unavailable." << std::endl;
    } else {
        std::cout << "[Engine] G-Buffer shaders loaded successfully!" << std::endl;

        // Create uniforms for G-Buffer shader (PBR material parameters)
        m_gbufferShader->createUniform("u_baseColor", bgfx::UniformType::Vec4);
        m_gbufferShader->createUniform("u_pbrParams", bgfx::UniformType::Vec4);  // metallic, roughness
        std::cout << "[Engine] G-Buffer shader uniforms created" << std::endl;
    }

    std::cout << "[Engine] Deferred renderer initialized - Toggle with 'D' key" << std::endl;
    std::cout << "[Engine] === DEFERRED RENDERING SYSTEM READY ===\n" << std::endl;

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

    // Setup camera GameObject with FPS controls
    std::cout << "[Scene Setup] Setting up FPS camera..." << std::endl;
    glm::vec3 cameraPos = m_pbrTestScenes->getCameraPositionForScene();

    // Create camera GameObject
    m_cameraObject = AnitoGameObjectManager::getInstance()->createObject("MainCamera", AnitoGameObject::PrimitiveType::Camera);
    m_cameraObject->setPosition(AnitoVector3D(cameraPos.x, cameraPos.y, cameraPos.z));

    // Add camera component
    m_camera = new AnitoCamera("MainCamera");
    m_cameraObject->attachComponent(m_camera);
    float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
    m_camera->setPerspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    // Add FPS camera control component
    auto* fpsControl = new AnitoFPSCameraControl("FPSControl");
    m_cameraObject->attachComponent(fpsControl);
    fpsControl->setMovementSpeed(5.0f);
    fpsControl->setLookSensitivity(0.1f);

    std::cout << "[Scene Setup] FPS Camera initialized at position: " 
              << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;

    // Set light direction (directional light from above-front-right)
    // Direction points FROM light source, intensity increased for PBR test visibility
    float lightDir[4] = { -0.3f, -0.5f, 0.8f, 12.0f }; // w = intensity
    shader->setUniform("u_lightDir", lightDir);

    std::cout << "[Scene Setup] PBR test scenes created successfully!" << std::endl;
    std::cout << "  - Current scene: " << m_pbrTestScenes->getCurrentSceneConfig().name << std::endl;
    std::cout << "  - Description: " << m_pbrTestScenes->getCurrentSceneConfig().description << std::endl;
    std::cout << "\n========== CONTROLS ==========" << std::endl;
    std::cout << "  Camera Movement:" << std::endl;
    std::cout << "    - WASD: Move forward/left/backward/right" << std::endl;
    std::cout << "    - Space/Left Ctrl: Move up/down" << std::endl;
    std::cout << "    - Left Shift: Sprint (2x speed)" << std::endl;
    std::cout << "    - Mouse: Look around" << std::endl;
    std::cout << "    - Scroll Wheel: Adjust movement speed" << std::endl;
    std::cout << "  Scene Controls:" << std::endl;
    std::cout << "    - SPACE: Switch between test scenes" << std::endl;
    std::cout << "    - Z: Toggle IBL (Image-Based Lighting)" << std::endl;
    std::cout << "    - D: Toggle Deferred Rendering (Forward/Deferred)" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "==================================================" << std::endl;
}

void AnitoEngine::run() {
    std::cout << "\n[Engine] Starting main loop..." << std::endl;

    // Reset elapsed runtime
    m_elapsedRuntime = 0.0f;

    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // Update elapsed runtime
        m_elapsedRuntime += deltaTime;

        // Check if we've exceeded the runtime limit
        if (m_maxRuntimeSeconds > 0.0f && m_elapsedRuntime >= m_maxRuntimeSeconds) {
            std::cout << "\n[Engine] Runtime limit reached (" << m_maxRuntimeSeconds 
                      << " seconds). Exiting..." << std::endl;
            m_running = false;
            break;
        }

        // Process events
        m_window->pollEvents();

        // Update
        update(deltaTime);

        // Render
        render();

        // Update frame capture recorder
        if (m_frameCaptureRecorder) {
            m_frameCaptureRecorder->update(deltaTime);
        }
    }

    std::cout << "[Engine] Main loop ended." << std::endl;
    std::cout << "[Engine] Total runtime: " << m_elapsedRuntime << " seconds" << std::endl;
}

void AnitoEngine::shutdown() {
    std::cout << "\n[Engine] Shutting down Anito Engine..." << std::endl;

    // Destroy PBR test scenes and frame capture recorder BEFORE bgfx shutdown
    // These hold bgfx resources (uniforms, textures, etc.) that must be destroyed before bgfx::shutdown()
    m_pbrTestScenes.reset();
    m_frameCaptureRecorder.reset();

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

    // Destroy profiling system (will automatically export final reports)
    std::cout << "[Engine] Exporting profiling data..." << std::endl;
    AnitoProfilerManager::destroy();

    std::cout << "[Engine] Shutdown complete." << std::endl;
}

void AnitoEngine::update(float deltaTime) {
    // Update input
    if (AnitoInputManager::getInstance()) {
        AnitoInputManager::getInstance()->update();

        // Handle scene switching with spacebar
        if (m_pbrTestScenes && AnitoInputManager::getInstance()->isKeyPressed(GLFW_KEY_SPACE)) {
            m_pbrTestScenes->switchToNextScene();

            // Update camera position for new scene (but keep current orientation)
            if (m_cameraObject) {
                glm::vec3 cameraPos = m_pbrTestScenes->getCameraPositionForScene();
                m_cameraObject->setPosition(AnitoVector3D(cameraPos.x, cameraPos.y, cameraPos.z));
                std::cout << "[Engine] Camera repositioned to: " 
                          << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;
            }
        }

        // [STEP 7] Handle deferred rendering toggle with 'D' key
        if (AnitoInputManager::getInstance()->isKeyPressed(GLFW_KEY_D)) {
            m_useDeferredRendering = !m_useDeferredRendering;
            std::cout << "\n[Engine] ========================================" << std::endl;
            std::cout << "[Engine] Rendering Mode: " << (m_useDeferredRendering ? "DEFERRED" : "FORWARD") << std::endl;
            std::cout << "[Engine] ========================================\n" << std::endl;
        }
    }

    // Update PBR test scenes (rotating cubes, etc.)
    if (m_pbrTestScenes) {
        m_pbrTestScenes->update(deltaTime);
    }

    // Update all game objects (includes camera with FPS controls)
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->updateAll(deltaTime);
    }
}

void AnitoEngine::render() {
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer || !m_camera) return;

    renderer->beginFrame();

    // Choose rendering path based on toggle
    if (m_useDeferredRendering && m_deferredRenderer && m_gbufferShader && m_gbufferShader->isValid()) {
        renderDeferred();
    } else {
        renderForward();
    }

    renderer->endFrame();
}

void AnitoEngine::renderForward() {
    // Original forward rendering path
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer || !m_camera) return;

    // Get camera view and projection matrices
    AnitoMatrix4x4 view = m_camera->getViewMatrix();
    AnitoMatrix4x4 proj = m_camera->getProjectionMatrix();

    // Set view-projection transform for the main view (View 1)
    bgfx::setViewTransform(renderer->getMainViewId(), view.data(), proj.data());

    // Get camera position for uniforms
    AnitoVector3D cameraPos = m_cameraObject->getPosition();
    float cameraPosArray[3] = { cameraPos.x(), cameraPos.y(), cameraPos.z() };

    // STEP 1: Render skybox first (View 0, background)
    if (renderer->isIBLReady() && renderer->getEnableIBL()) {
        // Calculate view-projection matrix for skybox
        glm::mat4 viewProj = glm::make_mat4(proj.data()) * glm::make_mat4(view.data());
        glm::mat4 viewProjInv = glm::inverse(viewProj);

        // Render skybox
        renderer->renderSkybox(glm::value_ptr(viewProjInv), cameraPosArray);
    }

    // Set global lighting uniforms for forward rendering
    if (m_pbrTestScenes) {
        auto shader = AnitoShader::get("SimpleShader");
        if (shader && shader->isValid()) {
            // Set light direction
            float lightDir[4] = { -0.3f, -0.5f, 0.8f, 12.0f };
            shader->setUniform("u_lightDir", lightDir);

            // Set camera position
            float cameraPosUniform[4] = { cameraPosArray[0], cameraPosArray[1], cameraPosArray[2], 1.0f };
            shader->setUniform("u_cameraPos", cameraPosUniform);
        }
    }

    // STEP 2: Render all game objects (View 1, foreground)
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::getInstance()->renderAll();
    }
}

void AnitoEngine::renderDeferred() {
    // [STEP 7] Deferred rendering path - Geometry pass only (lighting pass in Step 9)
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer || !m_camera) return;

    // Get camera view and projection matrices
    AnitoMatrix4x4 view = m_camera->getViewMatrix();
    AnitoMatrix4x4 proj = m_camera->getProjectionMatrix();

    // Get camera position
    AnitoVector3D cameraPos = m_cameraObject->getPosition();
    float cameraPosArray[3] = { cameraPos.x(), cameraPos.y(), cameraPos.z() };

    // STEP 1: Render skybox (View 0, background) - Same as forward
    if (renderer->isIBLReady() && renderer->getEnableIBL()) {
        glm::mat4 viewProj = glm::make_mat4(proj.data()) * glm::make_mat4(view.data());
        glm::mat4 viewProjInv = glm::inverse(viewProj);
        renderer->renderSkybox(glm::value_ptr(viewProjInv), cameraPosArray);
    }

    // STEP 2: Geometry Pass - Render scene objects to G-Buffer (View 2)
    std::cout << "[Engine] [STEP 7] Starting geometry pass..." << std::endl;

    m_deferredRenderer->beginGeometryPass();

    // Set view-projection transform for geometry view (View 2)
    bgfx::setViewTransform(m_deferredRenderer->getGeometryViewId(), view.data(), proj.data());

    // Set global uniforms for G-Buffer shader
    if (m_gbufferShader && m_gbufferShader->isValid()) {
        // Base color and PBR params will be set per-material in mesh renderer
        std::cout << "[Engine] G-Buffer shader ready for rendering" << std::endl;
    }

    // Render all game objects to G-Buffer
    // NOTE: This will use the current shader in each material (SimpleShader)
    // TODO (Step 7b): Need to temporarily swap shaders to GBufferShader
    if (AnitoGameObjectManager::getInstance()) {
        std::cout << "[Engine] Rendering scene objects to G-Buffer..." << std::endl;

        // TODO: We need a way to tell mesh renderers to use G-Buffer shader
        // For now, they will render with SimpleShader to View 2
        // This is a temporary limitation - we'll fix in next iteration

        AnitoGameObjectManager::getInstance()->renderAll();
    }

    m_deferredRenderer->endGeometryPass();
    std::cout << "[Engine] Geometry pass complete!" << std::endl;

    // STEP 3: Lighting Pass - NOT IMPLEMENTED YET (Step 9)
    // For now, just clear the screen so we can verify G-Buffer rendering
    // The G-Buffer will contain data but won't be visualized yet

    std::cout << "[Engine] [STEP 9 - NOT IMPLEMENTED] Lighting pass skipped" << std::endl;
    std::cout << "[Engine] G-Buffer populated but not visualized (waiting for Step 9)" << std::endl;
}

} // namespace Anito
