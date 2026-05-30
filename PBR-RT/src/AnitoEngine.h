#pragma once
#include <memory>
#include <string>
#include <bgfx/bgfx.h>

namespace Anito {

// Forward declarations for benchmark types
enum class BenchmarkDuration;
struct Resolution;
struct BenchmarkResults;

class AnitoWindow;
class AnitoRenderer;
class AnitoGameObjectManager;
class AnitoPBRTestScenes;
class AnitoFrameCaptureRecorder;
class AnitoGBuffer;
class AnitoDeferredRenderer;
class AnitoGameObject;
class AnitoCamera;
class AnitoShader;

/**
 * AnitoEngine - Main engine class
 * Coordinates all subsystems and main loop
 */
class AnitoEngine {
public:
    AnitoEngine();
    ~AnitoEngine();

    bool initialize(const std::string& title, uint32_t width = 1280, uint32_t height = 720);
    void run();
    void shutdown();

    static AnitoEngine* getInstance() { return s_instance; }

    // [STEP 12] Benchmark access methods
    AnitoWindow* getWindow() { return m_window; }
    AnitoFrameCaptureRecorder* getFrameCaptureRecorder() { return m_frameCaptureRecorder.get(); }
    bool isRunning() const { return m_running; }
    void stopRunning() { m_running = false; }

private:
    void update(float deltaTime);
    void render();
    void renderDeferred();  // Deferred rendering path
    void createTestScene();

    // [STEP 12] Benchmark mode execution
    void runBenchmarkMode();
    BenchmarkResults runSingleBenchmark(const Resolution& resolution, BenchmarkDuration duration);

    static AnitoEngine* s_instance;

    AnitoWindow* m_window;
    bool m_running;
    float m_lastFrameTime;

    // Runtime duration control (from engine_config.ini)
    float m_maxRuntimeSeconds;  // 0 = run indefinitely
    float m_elapsedRuntime;     // Total time elapsed since run() started
    float m_runtimeFeedbackIntervalSeconds; // Progress log cadence, 0 = disabled
    float m_nextRuntimeFeedbackTime;        // Next elapsed time to emit feedback

    // PBR test scenes system
    std::unique_ptr<AnitoPBRTestScenes> m_pbrTestScenes;

    // Frame capture for debugging
    std::unique_ptr<AnitoFrameCaptureRecorder> m_frameCaptureRecorder;

    // Camera GameObject with FPS control
    AnitoGameObject* m_cameraObject;
    AnitoCamera* m_camera;

    // Deferred rendering system (Phase 3)
    std::unique_ptr<AnitoDeferredRenderer> m_deferredRenderer;

    // G-Buffer shader (for deferred rendering - geometry pass)
    std::shared_ptr<AnitoShader> m_gbufferShader;

    // Deferred lighting shader (for deferred rendering - lighting pass, Step 9)
    bgfx::ProgramHandle m_deferredLightingProgram;
    bgfx::ProgramHandle m_gbufferDebugProgram;  // Step 10: Debug visualization
    bgfx::UniformHandle m_u_cameraPos;
    bgfx::UniformHandle m_u_lightDir;
    bgfx::UniformHandle m_u_enableIBL;
    bgfx::UniformHandle m_s_gbuffer0;
    bgfx::UniformHandle m_s_gbuffer1;
    bgfx::UniformHandle m_s_gbuffer2;
    bgfx::UniformHandle m_s_gbuffer3;
    bgfx::UniformHandle m_s_irradianceMap;
    bgfx::UniformHandle m_s_prefilterMap;
    bgfx::UniformHandle m_s_brdfLUT;

    // [STEP 12] Benchmark mode (Phase 3)
    bool m_benchmarkMode;  // If true, run benchmarks instead of normal execution
};

} // namespace Anito
