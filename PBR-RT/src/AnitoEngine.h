#pragma once
#include <memory>
#include <string>

namespace Anito {

class AnitoWindow;
class AnitoRenderer;
class AnitoGameObjectManager;
class AnitoPBRTestScenes;
class AnitoFrameCaptureRecorder;
class AnitoGBuffer;
class AnitoGameObject;
class AnitoCamera;

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

private:
    void update(float deltaTime);
    void render();
    void createTestScene();

    static AnitoEngine* s_instance;

    AnitoWindow* m_window;
    bool m_running;
    float m_lastFrameTime;

    // Runtime duration control (from engine_config.ini)
    float m_maxRuntimeSeconds;  // 0 = run indefinitely
    float m_elapsedRuntime;     // Total time elapsed since run() started

    // PBR test scenes system
    std::unique_ptr<AnitoPBRTestScenes> m_pbrTestScenes;

    // Frame capture for debugging
    std::unique_ptr<AnitoFrameCaptureRecorder> m_frameCaptureRecorder;

    // Camera GameObject with FPS control
    AnitoGameObject* m_cameraObject;
    AnitoCamera* m_camera;

    // Deferred rendering G-Buffer (Step 1-3 test)
    std::unique_ptr<AnitoGBuffer> m_gBuffer;

    // TODO: Add other subsystems as we implement them
};

} // namespace Anito
