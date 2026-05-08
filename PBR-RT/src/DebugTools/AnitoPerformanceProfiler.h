#pragma once
// Performance Profiler for CPU Timing
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <mutex>
#include <stack>

namespace Anito {

/**
 * AnitoPerformanceProfiler - Real-time CPU performance profiling
 * 
 * Features:
 * - Scoped profiling with ANITO_PROFILE_SCOPE macro
 * - Hierarchical timing tree (parent-child relationships)
 * - Min/Max/Average statistics over time windows
 * - Per-frame and cumulative metrics
 * - Export to JSON for AI agent feedback
 * - Chrome Tracing format support (view in chrome://tracing)
 * 
 * Future Implementations:
 * - ImGui real-time visualization
 * - Python scripts for AI analysis
 * - Memory allocation tracking integration
 * - Call stack visualization
 * - Performance regression detection
 */
class AnitoPerformanceProfiler {
public:
    struct ProfileScope {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double durationMs;
        int depth;
        int parentIndex;
    };

    struct ProfileStats {
        std::string name;
        int callCount;
        double totalMs;
        double minMs;
        double maxMs;
        double avgMs;
        double percentageOfFrame;
    };

    static AnitoPerformanceProfiler* getInstance();
    static void initialize(const std::string& outputDir = "anito-debug/profiling");
    static void destroy();

    /**
     * Begin a new profiling scope
     */
    void beginScope(const std::string& name);

    /**
     * End the current profiling scope
     */
    void endScope();

    /**
     * Begin a new frame (call at start of frame)
     */
    void beginFrame();

    /**
     * End the current frame (call at end of frame)
     */
    void endFrame();

    /**
     * Export profiling data to JSON
     * @param sessionDuration Total session duration in seconds
     */
    void exportToJSON(const std::string& filename, double sessionDuration);

    /**
     * Export profiling data to Chrome Tracing format
     */
    void exportToChromeTracing(const std::string& filename);

    /**
     * Get frame statistics
     */
    double getAverageFPS() const { return m_avgFPS; }
    double getAverageFrameTime() const { return m_avgFrameTime; }
    double getMinFrameTime() const { return m_minFrameTime; }
    double getMaxFrameTime() const { return m_maxFrameTime; }
    int getTotalFrames() const { return m_totalFrames; }

    /**
     * Get profiling statistics for a specific scope
     */
    bool getStats(const std::string& scopeName, ProfileStats& outStats) const;

    /**
     * Get all profiling statistics
     */
    std::vector<ProfileStats> getAllStats() const;

    /**
     * Clear all profiling data
     */
    void clear();

    /**
     * Enable/disable profiling
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    /**
     * Begin a benchmark session
     * @param resolution Screen resolution string (e.g., "1920x1080")
     * @param renderMode Render mode string ("Forward" or "Deferred")
     * @param buildConfig Build configuration ("Debug" or "Release")
     */
    void beginBenchmarkSession(
        const std::string& resolution,
        const std::string& renderMode,
        const std::string& buildConfig
    );

    /**
     * End benchmark session and return frame time samples for analysis
     * @return Vector of all frame times in milliseconds
     */
    std::vector<double> endBenchmarkSession();

    /**
     * Get frame time samples for percentile calculation
     */
    std::vector<double> getFrameTimeSamples() const;

    /**
     * Check if currently in benchmark session
     */
    bool isInBenchmarkSession() const { return m_inBenchmarkSession; }

private:
    AnitoPerformanceProfiler();
    ~AnitoPerformanceProfiler();
    AnitoPerformanceProfiler(const AnitoPerformanceProfiler&) = delete;
    AnitoPerformanceProfiler& operator=(const AnitoPerformanceProfiler&) = delete;

    void init(const std::string& outputDir);
    void release();
    void ensureOutputDirectoryExists(const std::string& path);
    void updateFrameStats();
    void exportToJSONUnlocked(const std::string& filename, double sessionDuration);

    static AnitoPerformanceProfiler* s_instance;

    std::string m_outputDir;
    std::mutex m_mutex;

    // Current frame profiling
    std::vector<ProfileScope> m_currentFrameScopes;
    std::stack<int> m_scopeStack;
    int m_currentDepth;

    // Accumulated statistics
    std::unordered_map<std::string, ProfileStats> m_stats;

    // Frame timing
    std::chrono::high_resolution_clock::time_point m_frameStartTime;
    std::chrono::high_resolution_clock::time_point m_sessionStartTime;
    double m_currentFrameTime;
    double m_avgFrameTime;
    double m_minFrameTime;
    double m_maxFrameTime;
    double m_avgFPS;
    int m_totalFrames;

    // All frames history (for export)
    std::vector<std::vector<ProfileScope>> m_frameHistory;
    static constexpr int MAX_FRAME_HISTORY = 600; // Keep last 10 seconds at 60fps

    bool m_enabled;
    bool m_inFrame;

    // Benchmark session tracking
    bool m_inBenchmarkSession;
    std::string m_benchmarkResolution;
    std::string m_benchmarkRenderMode;
    std::string m_benchmarkBuildConfig;
    std::vector<double> m_frameTimeSamples; // For percentile calculation
};

/**
 * RAII helper for automatic scope profiling
 */
class AnitoProfileScopeRAII {
public:
    explicit AnitoProfileScopeRAII(const char* name) {
        if (AnitoPerformanceProfiler::getInstance() && 
            AnitoPerformanceProfiler::getInstance()->isEnabled()) {
            AnitoPerformanceProfiler::getInstance()->beginScope(name);
        }
    }

    ~AnitoProfileScopeRAII() {
        if (AnitoPerformanceProfiler::getInstance() && 
            AnitoPerformanceProfiler::getInstance()->isEnabled()) {
            AnitoPerformanceProfiler::getInstance()->endScope();
        }
    }
};

} // namespace Anito

// Convenient macros for profiling
#ifdef ANITO_ENABLE_PROFILING
    #define ANITO_PROFILE_SCOPE(name) Anito::AnitoProfileScopeRAII __profiler_scope__(name)
    #define ANITO_PROFILE_FUNCTION() ANITO_PROFILE_SCOPE(__FUNCTION__)
#else
    #define ANITO_PROFILE_SCOPE(name)
    #define ANITO_PROFILE_FUNCTION()
#endif
