#pragma once
// Frame Capture Recorder for Debug Purposes
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <cstdint>
#include <vector>
#include <bgfx/bgfx.h>

namespace Anito {

struct PendingConversion {
    std::string tgaPath;
    std::string targetPath;
    int frameNum;
};

/**
 * AnitoFrameCaptureRecorder - Captures frames as PNG/JPG images
 * 
 * Features:
 * - Automatic frame capture at specified intervals
 * - Saves to anito-debug directory
 * - Sequential frame numbering (frame_0, frame_1, etc.)
 * - PNG and JPG support
 */
class AnitoFrameCaptureRecorder {
public:
    AnitoFrameCaptureRecorder();
    ~AnitoFrameCaptureRecorder();

    /**
     * Initialize the frame capture recorder
     * @param outputDir Directory to save frames (default: "anito-debug/frames")
     * @param captureIntervalSeconds Time between captures in seconds
     * @param format Image format ("png" or "jpg")
     * @param jpgQuality Quality for JPG compression (1-100, default 90)
     */
    void initialize(const std::string& outputDir = "anito-debug/frames", 
                    float captureIntervalSeconds = 3.0f,
                    const std::string& format = "png",
                    int jpgQuality = 90);

    /**
     * Update the recorder (call every frame)
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime);

    /**
     * Manually trigger a frame capture
     */
    void captureFrame();

    /**
     * Get the number of frames captured
     */
    int getFrameCount() const { return m_frameCount; }

    /**
     * Enable/disable automatic captures
     */
    void setAutoCapture(bool enabled) { m_autoCapture = enabled; }

    /**
     * Get the output directory
     */
    const std::string& getOutputDirectory() const { return m_outputDir; }

private:
    void ensureOutputDirectoryExists();
    void processConversions();
    bool convertTgaToPngOrJpg(const std::string& tgaPath, const std::string& targetPath);

    std::string m_outputDir;
    std::string m_format;
    int m_jpgQuality;
    float m_captureInterval;
    float m_timeSinceLastCapture;
    int m_frameCount;
    bool m_autoCapture;
    uint32_t m_width;
    uint32_t m_height;

    std::string m_pendingCapture;
    std::vector<PendingConversion> m_pendingConversions;
};

} // namespace Anito
