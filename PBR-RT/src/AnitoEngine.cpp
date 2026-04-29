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
#include "Benchmarking/AnitoBenchmarkTest.h"
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
#include <algorithm>
#include <numeric>

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
    , m_deferredLightingProgram(BGFX_INVALID_HANDLE)
    , m_gbufferDebugProgram(BGFX_INVALID_HANDLE)
    , m_u_cameraPos(BGFX_INVALID_HANDLE)
    , m_s_gbuffer0(BGFX_INVALID_HANDLE)
    , m_s_gbuffer1(BGFX_INVALID_HANDLE)
    , m_s_gbuffer2(BGFX_INVALID_HANDLE)
    , m_s_gbuffer3(BGFX_INVALID_HANDLE)
    , m_s_irradianceMap(BGFX_INVALID_HANDLE)
    , m_s_prefilterMap(BGFX_INVALID_HANDLE)
    , m_s_brdfLUT(BGFX_INVALID_HANDLE)
    , m_benchmarkMode(false)  // [STEP 12] Default to normal operation
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

    // [STEP 12] Check if benchmark mode is enabled
    m_benchmarkMode = AnitoEngineConfig::getBool("Benchmark.EnableBenchmarkMode", false);

    if (m_benchmarkMode) {
        std::cout << "\n========================================================" << std::endl;
        std::cout << "  [BENCHMARK MODE] Phase 3, Step 12" << std::endl;
        std::cout << "  Automated Performance Testing - Deferred Rendering" << std::endl;
        std::cout << "========================================================\n" << std::endl;

        // Parse benchmark duration from config
        std::string durationStr = AnitoEngineConfig::getString("Benchmark.BenchmarkDuration", "Quick");
        BenchmarkDuration duration = BenchmarkDuration::Quick;
        if (durationStr == "Standard") {
            duration = BenchmarkDuration::Standard;
        } else if (durationStr == "Extended") {
            duration = BenchmarkDuration::Extended;
        }

        // Force deferred rendering for benchmarking (as user requested)
        m_useDeferredRendering = true;
        std::cout << "[Benchmark] Forcing deferred rendering mode (runtime toggle is buggy)" << std::endl;
    }

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

    // [STEP 8-9] Load deferred lighting shaders
    std::cout << "[Engine] Step 8-9: Loading deferred lighting shaders..." << std::endl;

    // Create lighting program directly using bgfx (not AnitoShader wrapper)
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (renderer) {
        bgfx::ShaderHandle vsDeferred = renderer->createShader("assets/shaders/compiled/vs_deferred_light.bin");
        bgfx::ShaderHandle fsDeferred = renderer->createShader("assets/shaders/compiled/fs_deferred_light.bin");

        if (bgfx::isValid(vsDeferred) && bgfx::isValid(fsDeferred)) {
            m_deferredLightingProgram = bgfx::createProgram(vsDeferred, fsDeferred, true);

            if (bgfx::isValid(m_deferredLightingProgram)) {
                std::cout << "[Engine] Deferred lighting shaders loaded successfully!" << std::endl;

                // Create uniforms for lighting pass
                m_u_cameraPos = bgfx::createUniform("u_cameraPos", bgfx::UniformType::Vec4);
                m_s_gbuffer0 = bgfx::createUniform("s_gbuffer0", bgfx::UniformType::Sampler);
                m_s_gbuffer1 = bgfx::createUniform("s_gbuffer1", bgfx::UniformType::Sampler);
                m_s_gbuffer2 = bgfx::createUniform("s_gbuffer2", bgfx::UniformType::Sampler);
                m_s_gbuffer3 = bgfx::createUniform("s_gbuffer3", bgfx::UniformType::Sampler);
                m_s_irradianceMap = bgfx::createUniform("s_irradianceMap", bgfx::UniformType::Sampler);
                m_s_prefilterMap = bgfx::createUniform("s_prefilterMap", bgfx::UniformType::Sampler);
                m_s_brdfLUT = bgfx::createUniform("s_brdfLUT", bgfx::UniformType::Sampler);

                std::cout << "[Engine] Deferred lighting uniforms created" << std::endl;

                // Configure deferred renderer with shader and uniforms
                m_deferredRenderer->setLightingShader(m_deferredLightingProgram);
                m_deferredRenderer->setGBufferUniforms(m_s_gbuffer0, m_s_gbuffer1, m_s_gbuffer2, m_s_gbuffer3);
                m_deferredRenderer->setCameraUniforms(m_u_cameraPos);
                m_deferredRenderer->setIBLUniforms(m_s_irradianceMap, m_s_prefilterMap, m_s_brdfLUT);
            } else {
                std::cerr << "[Engine] ERROR: Failed to create deferred lighting program!" << std::endl;
            }
        } else {
            std::cerr << "[Engine] ERROR: Failed to load deferred lighting shaders!" << std::endl;
        }

        // [STEP 10] Load G-Buffer debug visualization shaders
        std::cout << "[Engine] Step 10: Loading G-Buffer debug visualization shaders..." << std::endl;
        bgfx::ShaderHandle vsDebug = renderer->createShader("assets/shaders/compiled/vs_gbuffer_debug.bin");
        bgfx::ShaderHandle fsDebug = renderer->createShader("assets/shaders/compiled/fs_gbuffer_debug.bin");

        if (bgfx::isValid(vsDebug) && bgfx::isValid(fsDebug)) {
            m_gbufferDebugProgram = bgfx::createProgram(vsDebug, fsDebug, true);

            if (bgfx::isValid(m_gbufferDebugProgram)) {
                std::cout << "[Engine] G-Buffer debug shaders loaded successfully!" << std::endl;
                m_deferredRenderer->setDebugShader(m_gbufferDebugProgram);
            } else {
                std::cerr << "[Engine] ERROR: Failed to create G-Buffer debug program!" << std::endl;
            }
        } else {
            std::cerr << "[Engine] ERROR: Failed to load G-Buffer debug shaders!" << std::endl;
        }
    }

    std::cout << "[Engine] Deferred renderer initialized - Toggle with 'D' key" << std::endl;
    std::cout << "[Engine] G-Buffer debug visualization - Toggle with 'G' key" << std::endl;
    std::cout << "[Engine] === DEFERRED RENDERING SYSTEM READY ===\n" << std::endl;

    // Create test scene
    createTestScene();

    m_running = true;
    m_lastFrameTime = static_cast<float>(glfwGetTime());

    std::cout << "Anito Engine initialized successfully!" << std::endl;
    std::cout << "==================================================" << std::endl;

    // [STEP 12] If benchmark mode is enabled, DON'T run benchmarks in initialize()
    // Instead, set a flag and let run() handle it
    if (m_benchmarkMode) {
        std::cout << "\n========================================================" << std::endl;
        std::cout << "  [BENCHMARK MODE ENABLED]" << std::endl;
        std::cout << "  Benchmarks will run after initialization" << std::endl;
        std::cout << "========================================================\n" << std::endl;
    }

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
    glm::vec3 lookAtPos = m_pbrTestScenes->getLookAtPositionForScene();

    // Create camera GameObject
    m_cameraObject = AnitoGameObjectManager::getInstance()->createObject("MainCamera", AnitoGameObject::PrimitiveType::Camera);
    m_cameraObject->setPosition(AnitoVector3D(cameraPos.x, cameraPos.y, cameraPos.z));

    // Add camera component
    m_camera = new AnitoCamera("MainCamera");
    m_cameraObject->attachComponent(m_camera);
    float aspect = static_cast<float>(renderer->getWidth()) / static_cast<float>(renderer->getHeight());
    m_camera->setPerspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);

    // Add FPS camera control component and initialize orientation to look at scene
    auto* fpsControl = new AnitoFPSCameraControl("FPSControl");
    m_cameraObject->attachComponent(fpsControl);
    fpsControl->setMovementSpeed(5.0f);
    fpsControl->setLookSensitivity(0.1f);

    // Calculate initial yaw and pitch to look at scene center
    glm::vec3 direction = glm::normalize(lookAtPos - cameraPos);
    float yaw = glm::degrees(atan2(direction.x, direction.z));
    float pitch = glm::degrees(asin(-direction.y));
    fpsControl->setYaw(yaw);
    fpsControl->setPitch(pitch);

    std::cout << "[Scene Setup] FPS Camera initialized at position: " 
              << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;
    std::cout << "[Scene Setup] Looking at: " 
              << lookAtPos.x << ", " << lookAtPos.y << ", " << lookAtPos.z << std::endl;
    std::cout << "[Scene Setup] Initial orientation - Yaw: " << yaw << "°, Pitch: " << pitch << "°" << std::endl;

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
    std::cout << "    - G: Toggle G-Buffer Debug Visualization" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << "==================================================" << std::endl;
}

void AnitoEngine::run() {
    std::cout << "\n[Engine] Starting main loop..." << std::endl;

    // [STEP 12] If benchmark mode is enabled, run benchmarks instead
    if (m_benchmarkMode) {
        runBenchmarkMode();
        return;
    }

    // Reset elapsed runtime
    m_elapsedRuntime = 0.0f;

    // Get CPU profiler for frame timing
    auto* cpuProfiler = AnitoProfilerManager::getCPUProfiler();

    while (m_running && !m_window->shouldClose()) {
        // Begin frame profiling
        if (cpuProfiler) {
            cpuProfiler->beginFrame();
        }

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

        // End frame profiling
        if (cpuProfiler) {
            cpuProfiler->endFrame();
        }
    }

    std::cout << "[Engine] Main loop ended." << std::endl;
    std::cout << "[Engine] Total runtime: " << m_elapsedRuntime << " seconds" << std::endl;
}

void AnitoEngine::runBenchmarkMode() {
    std::cout << "\n[Benchmark] ===========================================" << std::endl;
    std::cout << "[Benchmark]   Starting Benchmark Execution" << std::endl;
    std::cout << "[Benchmark] ===========================================" << std::endl;

    // Parse benchmark configuration
    std::string durationStr = AnitoEngineConfig::getString("Benchmark.BenchmarkDuration", "Quick");
    BenchmarkDuration duration = BenchmarkDuration::Quick;
    if (durationStr == "Standard") {
        duration = BenchmarkDuration::Standard;
    } else if (durationStr == "Extended") {
        duration = BenchmarkDuration::Extended;
    }

    // Detect build configuration
#ifdef _DEBUG
    std::string buildConfig = "Debug";
#else
    std::string buildConfig = "Release";
#endif

    std::cout << "[Benchmark] Configuration:" << std::endl;
    std::cout << "  - Duration: " << durationStr << " (" << BenchmarkConfig().getDurationSeconds() << "s per test)" << std::endl;
    std::cout << "  - Build: " << buildConfig << std::endl;
    std::cout << "  - Resolutions: 720p, 1080p, 1440p" << std::endl;
    std::cout << "[Benchmark] ===========================================\n" << std::endl;

    // Define test resolutions
    std::vector<Resolution> resolutions = {
        Resolution{1280, 720},
        Resolution{1920, 1080},
        Resolution{2560, 1440}
    };

    std::vector<BenchmarkResults> allResults;

    // Run benchmark for each resolution
    for (const auto& resolution : resolutions) {
        std::cout << "\n[Benchmark] -------------------------------------------" << std::endl;
        std::cout << "[Benchmark] Testing Resolution: " << resolution.width << "x" << resolution.height << std::endl;
        std::cout << "[Benchmark] -------------------------------------------" << std::endl;

        // Resize window
        if (m_window) {
            m_window->resize(resolution.width, resolution.height);
            std::cout << "[Benchmark] Window resized to " << resolution.width << "x" << resolution.height << std::endl;
        }

        // Run benchmark for this resolution
        BenchmarkResults results = runSingleBenchmark(resolution, duration);
        results.buildConfig = buildConfig;
        allResults.push_back(results);

        // Print results
        std::cout << "[Benchmark] Results:" << std::endl;
        std::cout << "  - Avg FPS: " << results.avgFPS << std::endl;
        std::cout << "  - Min FPS: " << results.minFPS << std::endl;
        std::cout << "  - Max FPS: " << results.maxFPS << std::endl;
        std::cout << "  - Avg Frame Time: " << results.avgFrameTimeMs << " ms" << std::endl;
        std::cout << "  - 95th Percentile: " << results.percentile95Ms << " ms" << std::endl;
        std::cout << "  - Total Frames: " << results.totalFrames << std::endl;
        std::cout << "  - Performance Target: " << (results.meetsPerformanceTarget ? "PASS" : "FAIL") << std::endl;
    }

    // Export results
    std::cout << "\n[Benchmark] Exporting results..." << std::endl;
    AnitoBenchmarkTest benchmark;
    benchmark.exportToJSON(allResults, "anito-debug/benchmark_results.json");
    benchmark.exportToCSV(allResults, "anito-debug/benchmark_results.csv");
    benchmark.printComparisonReport(allResults);

    std::cout << "\n[Benchmark] ===========================================" << std::endl;
    std::cout << "[Benchmark]   Benchmark Complete!" << std::endl;
    std::cout << "[Benchmark]   Results saved to anito-debug/" << std::endl;
    std::cout << "[Benchmark] ===========================================" << std::endl;

    m_running = false;  // Exit after benchmarking
}

BenchmarkResults AnitoEngine::runSingleBenchmark(const Resolution& resolution, BenchmarkDuration duration) {
    BenchmarkResults results;
    results.resolution = std::to_string(resolution.width) + "x" + std::to_string(resolution.height);
    results.renderMode = "Deferred";  // Always deferred in benchmark mode

    // Get benchmark duration
    float durationSeconds = 30.0f;  // Quick
    switch (duration) {
        case BenchmarkDuration::Standard: durationSeconds = 60.0f; break;
        case BenchmarkDuration::Extended: durationSeconds = 120.0f; break;
        default: break;
    }

    std::cout << "[Benchmark] Running for " << durationSeconds << " seconds..." << std::endl;

    // Get CPU profiler
    auto* cpuProfiler = AnitoProfilerManager::getCPUProfiler();
    if (cpuProfiler) {
        cpuProfiler->beginBenchmarkSession(results.resolution, results.renderMode, results.buildConfig);
    }

    // Disable frame capture during benchmark (too much overhead)
    bool originalCaptureState = false;
    if (m_frameCaptureRecorder) {
        originalCaptureState = true;  // Store state
        m_frameCaptureRecorder->setAutoCapture(false);
    }

    // Run benchmark loop
    float elapsed = 0.0f;
    float lastFrameTime = static_cast<float>(glfwGetTime());
    int frameCount = 0;

    while (elapsed < durationSeconds && !m_window->shouldClose()) {
        // Begin frame profiling
        if (cpuProfiler) {
            cpuProfiler->beginFrame();
        }

        // Calculate delta time
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        elapsed += deltaTime;

        // Process events
        m_window->pollEvents();

        // Update (minimal updates for benchmarking)
        if (m_pbrTestScenes) {
            m_pbrTestScenes->update(deltaTime);
        }
        if (AnitoGameObjectManager::getInstance()) {
            AnitoGameObjectManager::getInstance()->updateAll(deltaTime);
        }

        // Render
        render();

        // End frame profiling
        if (cpuProfiler) {
            cpuProfiler->endFrame();
        }

        frameCount++;
    }

    std::cout << "[Benchmark] Completed " << frameCount << " frames in " << elapsed << " seconds" << std::endl;

    // Restore frame capture state
    if (m_frameCaptureRecorder) {
        m_frameCaptureRecorder->setAutoCapture(originalCaptureState);
    }

    // Get profiling samples
    std::vector<double> frameTimeSamples;
    if (cpuProfiler) {
        cpuProfiler->endBenchmarkSession();
        frameTimeSamples = cpuProfiler->getFrameTimeSamples();
    }

    // Calculate statistics
    if (!frameTimeSamples.empty()) {
        std::sort(frameTimeSamples.begin(), frameTimeSamples.end());

        double sum = std::accumulate(frameTimeSamples.begin(), frameTimeSamples.end(), 0.0);
        results.avgFrameTimeMs = (sum / frameTimeSamples.size()) * 1000.0;
        results.minFrameTimeMs = frameTimeSamples.front() * 1000.0;
        results.maxFrameTimeMs = frameTimeSamples.back() * 1000.0;

        results.avgFPS = 1000.0 / results.avgFrameTimeMs;
        results.minFPS = 1000.0 / results.maxFrameTimeMs;
        results.maxFPS = 1000.0 / results.minFrameTimeMs;

        size_t p95Index = static_cast<size_t>(frameTimeSamples.size() * 0.95);
        size_t p99Index = static_cast<size_t>(frameTimeSamples.size() * 0.99);
        results.percentile95Ms = frameTimeSamples[p95Index] * 1000.0;
        results.percentile99Ms = frameTimeSamples[p99Index] * 1000.0;

        results.totalFrames = static_cast<int>(frameTimeSamples.size());
        results.framesUnder16ms = static_cast<int>(std::count_if(frameTimeSamples.begin(), frameTimeSamples.end(),
            [](double t) { return (t * 1000.0) < 16.67; }));
        results.droppedFrames = results.totalFrames - results.framesUnder16ms;
        results.meetsPerformanceTarget = (results.avgFPS >= 60.0);
    } else {
        std::cout << "[Benchmark] WARNING: No profiler samples collected" << std::endl;
        results.avgFPS = (frameCount / elapsed);
        results.avgFrameTimeMs = (elapsed / frameCount) * 1000.0;
        results.totalFrames = frameCount;
        results.meetsPerformanceTarget = (results.avgFPS >= 60.0);
    }

    results.gpuMemoryUsedMB = 0;  // TODO
    results.systemMemoryUsedMB = 0;  // TODO

    return results;
}

void AnitoEngine::shutdown() {
    std::cout << "\n[Engine] Shutting down Anito Engine..." << std::endl;

    // Destroy deferred rendering resources BEFORE bgfx shutdown
    if (bgfx::isValid(m_deferredLightingProgram)) {
        bgfx::destroy(m_deferredLightingProgram);
    }
    if (bgfx::isValid(m_gbufferDebugProgram)) {
        bgfx::destroy(m_gbufferDebugProgram);
    }
    if (bgfx::isValid(m_u_cameraPos)) bgfx::destroy(m_u_cameraPos);
    if (bgfx::isValid(m_s_gbuffer0)) bgfx::destroy(m_s_gbuffer0);
    if (bgfx::isValid(m_s_gbuffer1)) bgfx::destroy(m_s_gbuffer1);
    if (bgfx::isValid(m_s_gbuffer2)) bgfx::destroy(m_s_gbuffer2);
    if (bgfx::isValid(m_s_gbuffer3)) bgfx::destroy(m_s_gbuffer3);
    if (bgfx::isValid(m_s_irradianceMap)) bgfx::destroy(m_s_irradianceMap);
    if (bgfx::isValid(m_s_prefilterMap)) bgfx::destroy(m_s_prefilterMap);
    if (bgfx::isValid(m_s_brdfLUT)) bgfx::destroy(m_s_brdfLUT);

    // Destroy PBR test scenes and frame capture recorder BEFORE bgfx shutdown
    // These hold bgfx resources (uniforms, textures, etc.) that must be destroyed before bgfx::shutdown()
    m_pbrTestScenes.reset();
    m_frameCaptureRecorder.reset();

    // Destroy game objects
    if (AnitoGameObjectManager::getInstance()) {
        AnitoGameObjectManager::destroy();
    }

    // [FIX] Destroy shader member variables BEFORE bgfx shutdown
    // m_gbufferShader holds bgfx uniform handles that must be destroyed while bgfx context is still valid
    // If we don't reset this here, it gets destroyed in ~AnitoEngine() after bgfx::shutdown(),
    // causing access violation when trying to lock bgfx::s_ctx->m_resourceApiLock (s_ctx is NULL)
    m_gbufferShader.reset();

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

            // Update camera position and orientation for new scene
            if (m_cameraObject) {
                glm::vec3 cameraPos = m_pbrTestScenes->getCameraPositionForScene();
                glm::vec3 lookAtPos = m_pbrTestScenes->getLookAtPositionForScene();
                m_cameraObject->setPosition(AnitoVector3D(cameraPos.x, cameraPos.y, cameraPos.z));

                // Update FPS camera control orientation
                auto* fpsControl = m_cameraObject->getComponent<AnitoFPSCameraControl>();
                if (fpsControl) {
                    glm::vec3 direction = glm::normalize(lookAtPos - cameraPos);
                    float yaw = glm::degrees(atan2(direction.x, direction.z));
                    float pitch = glm::degrees(asin(-direction.y));
                    fpsControl->setYaw(yaw);
                    fpsControl->setPitch(pitch);
                }

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

        // [STEP 10] Handle G-Buffer debug visualization toggle with 'G' key
        if (AnitoInputManager::getInstance()->isKeyPressed(GLFW_KEY_G)) {
            if (m_deferredRenderer) {
                bool debugMode = !m_deferredRenderer->getDebugMode();
                m_deferredRenderer->setDebugMode(debugMode);
                std::cout << "\n[Engine] ========================================" << std::endl;
                std::cout << "[Engine] G-Buffer Debug Mode: " << (debugMode ? "ON" : "OFF") << std::endl;
                std::cout << "[Engine] ========================================\n" << std::endl;
            }
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

    // STEP 3: Lighting Pass (Step 9) OR Debug Visualization (Step 10)
    if (m_deferredRenderer->getDebugMode()) {
        // [STEP 10] Render G-Buffer debug visualization
        std::cout << "[Engine] [STEP 10] Rendering G-Buffer debug visualization..." << std::endl;
        m_deferredRenderer->renderDebugVisualization();
    } else {
        // [STEP 9] Normal lighting pass
        std::cout << "[Engine] [STEP 9] Starting lighting pass..." << std::endl;

        m_deferredRenderer->beginLightingPass();

        // Set view transform for lighting pass (identity - fullscreen pass)
        float identity[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        bgfx::setViewTransform(m_deferredRenderer->getLightingViewId(), identity, identity);

        // Set camera position uniform
        float cameraPosVec4[4] = { cameraPos.x(), cameraPos.y(), cameraPos.z(), 1.0f };
        bgfx::setUniform(m_u_cameraPos, cameraPosVec4);

        // Bind IBL textures if available
        if (renderer && renderer->isIBLReady()) {
            bgfx::setTexture(4, m_s_irradianceMap, renderer->getIrradianceMap());
            bgfx::setTexture(5, m_s_prefilterMap, renderer->getPrefilterMap());
            // Note: s_brdfLUT not implemented yet (would need BRDF LUT texture generation)
        }

        m_deferredRenderer->endLightingPass();
        std::cout << "[Engine] Lighting pass complete!" << std::endl;
    }
}

} // namespace Anito
