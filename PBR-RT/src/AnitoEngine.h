#pragma once
#include <memory>
#include <string>

namespace Anito {

class AnitoWindow;
class AnitoRenderer;
class AnitoGameObjectManager;
class AnitoPBRTestScenes;
class AnitoFrameCaptureRecorder;

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

    /**
     * Enable auto-shutdown after specified seconds (for debugging/testing)
     * @param seconds Time in seconds before auto-shutdown (0 = disabled)
     */
    void setAutoShutdown(float seconds) { m_autoShutdownTime = seconds; }

    static AnitoEngine* getInstance() { return s_instance; }

private:
    void update(float deltaTime);
    void render();
    void createTestScene();

    static AnitoEngine* s_instance;

    AnitoWindow* m_window;
    bool m_running;
    float m_lastFrameTime;

    // PBR test scenes system
    std::unique_ptr<AnitoPBRTestScenes> m_pbrTestScenes;

    // Frame capture for debugging
    std::unique_ptr<AnitoFrameCaptureRecorder> m_frameCaptureRecorder;
    float m_engineStartTime;
    float m_autoShutdownTime; // 0 = disabled, > 0 = shutdown after this many seconds

    // Current camera position (updated per scene)
    float m_currentCameraPos[3];

    // TODO: Add other subsystems as we implement them
};

} // namespace Anito
