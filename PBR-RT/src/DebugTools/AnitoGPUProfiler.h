#pragma once
// GPU Profiler for Rendering Performance
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <bgfx/bgfx.h>

namespace Anito {

/**
 * AnitoGPUProfiler - GPU performance profiling for rendering optimization
 * 
 * Features:
 * - GPU query-based timing (using bgfx queries)
 * - Per-pass rendering times (GBuffer, lighting, post-processing)
 * - Draw call tracking and batch statistics
 * - Texture/buffer memory usage tracking
 * - Export to JSON/CSV for AI agent feedback
 * 
 * Implementation Notes:
 * - Uses platform-specific GPU timer queries
 * - On Windows: D3D11/D3D12 timestamp queries via bgfx
 * - Results are asynchronous (available 1-2 frames later)
 * 
 * Future Implementations:
 * - ImGui real-time visualization
 * - Overdraw and fillrate estimation
 * - Pipeline state change tracking
 * - Shader compilation time tracking
 * - Integration with NVIDIA Nsight/RenderDoc
 */
class AnitoGPUProfiler {
public:
    struct GPUPassStats {
        std::string name;
        double gpuTimeMs;
        int drawCalls;
        int triangles;
        int vertices;
    };

    struct GPUFrameStats {
        double totalGPUTimeMs;
        int totalDrawCalls;
        int totalTriangles;
        int totalVertices;
        int textureMemoryMB;
        int bufferMemoryMB;
        std::vector<GPUPassStats> passes;
    };

    static AnitoGPUProfiler* getInstance();
    static void initialize(const std::string& outputDir = "anito-debug/profiling");
    static void destroy();

    /**
     * Begin GPU profiling for the current frame
     */
    void beginFrame();

    /**
     * End GPU profiling for the current frame
     */
    void endFrame();

    /**
     * Begin a GPU pass (e.g., "GBuffer", "Lighting", "Shadow Maps")
     */
    void beginPass(const std::string& passName);

    /**
     * End the current GPU pass
     */
    void endPass();

    /**
     * Record draw call statistics
     */
    void recordDrawCall(int triangles, int vertices);

    /**
     * Update texture memory usage
     */
    void setTextureMemoryUsage(int memoryMB) { m_textureMemoryMB = memoryMB; }

    /**
     * Update buffer memory usage
     */
    void setBufferMemoryUsage(int memoryMB) { m_bufferMemoryMB = memoryMB; }

    /**
     * Get last frame statistics
     */
    const GPUFrameStats& getLastFrameStats() const { return m_lastFrameStats; }

    /**
     * Get average GPU time over last N frames
     */
    double getAverageGPUTime() const { return m_avgGPUTime; }

    /**
     * Export profiling data to JSON
     */
    void exportToJSON(const std::string& filename);

    /**
     * Export profiling data to CSV
     */
    void exportToCSV(const std::string& filename);

    /**
     * Enable/disable GPU profiling
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    /**
     * Print frame statistics to console
     */
    void printFrameStats() const;

private:
    AnitoGPUProfiler();
    ~AnitoGPUProfiler();
    AnitoGPUProfiler(const AnitoGPUProfiler&) = delete;
    AnitoGPUProfiler& operator=(const AnitoGPUProfiler&) = delete;

    void init(const std::string& outputDir);
    void release();
    void ensureOutputDirectoryExists(const std::string& path);
    void updateAverageGPUTime();

    static AnitoGPUProfiler* s_instance;

    std::string m_outputDir;
    std::mutex m_mutex;

    // Current frame tracking
    std::string m_currentPassName;
    GPUPassStats m_currentPass;
    GPUFrameStats m_currentFrame;
    GPUFrameStats m_lastFrameStats;

    // Accumulated statistics
    std::vector<GPUFrameStats> m_frameHistory;
    static constexpr int MAX_FRAME_HISTORY = 600; // 10 seconds at 60fps

    double m_avgGPUTime;
    int m_totalFrames;

    // Memory tracking
    int m_textureMemoryMB;
    int m_bufferMemoryMB;

    bool m_enabled;
    bool m_inFrame;
    bool m_inPass;

    // Note: GPU timer queries are platform-specific and handled by bgfx internally
    // We track statistics manually since bgfx doesn't expose direct query API
};

} // namespace Anito

// Convenient macros for GPU profiling
#ifdef ANITO_ENABLE_GPU_PROFILING
    #define ANITO_GPU_BEGIN_FRAME() \
        if (Anito::AnitoGPUProfiler::getInstance()) \
            Anito::AnitoGPUProfiler::getInstance()->beginFrame()

    #define ANITO_GPU_END_FRAME() \
        if (Anito::AnitoGPUProfiler::getInstance()) \
            Anito::AnitoGPUProfiler::getInstance()->endFrame()

    #define ANITO_GPU_BEGIN_PASS(name) \
        if (Anito::AnitoGPUProfiler::getInstance()) \
            Anito::AnitoGPUProfiler::getInstance()->beginPass(name)

    #define ANITO_GPU_END_PASS() \
        if (Anito::AnitoGPUProfiler::getInstance()) \
            Anito::AnitoGPUProfiler::getInstance()->endPass()

    #define ANITO_GPU_RECORD_DRAW(triangles, vertices) \
        if (Anito::AnitoGPUProfiler::getInstance()) \
            Anito::AnitoGPUProfiler::getInstance()->recordDrawCall(triangles, vertices)
#else
    #define ANITO_GPU_BEGIN_FRAME()
    #define ANITO_GPU_END_FRAME()
    #define ANITO_GPU_BEGIN_PASS(name)
    #define ANITO_GPU_END_PASS()
    #define ANITO_GPU_RECORD_DRAW(triangles, vertices)
#endif
