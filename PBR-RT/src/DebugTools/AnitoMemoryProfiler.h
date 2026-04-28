#pragma once
// Memory Profiler for Allocation Tracking
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <cstdint>

namespace Anito {

/**
 * AnitoMemoryProfiler - Memory allocation tracking and leak detection
 * 
 * Features:
 * - Three tracking modes: None, Light, Heavy
 * - Per-system memory budgets tracking
 * - Live memory usage statistics
 * - Leak detection on shutdown
 * - Export to JSON for AI agent feedback
 * 
 * Tracking Modes:
 * - NONE: No tracking (production builds)
 * - LIGHT: Basic statistics only (allocation counts, total size)
 * - HEAVY: Full tracking with stack traces (debug builds only)
 * 
 * Future Implementations:
 * - ImGui real-time visualization
 * - Allocation stack traces in HEAVY mode
 * - Memory pool statistics
 * - Fragmentation analysis
 * - Platform-specific heap tracking (Android)
 */
class AnitoMemoryProfiler {
public:
    enum class TrackingMode {
        NONE = 0,   // No tracking
        LIGHT = 1,  // Basic statistics only
        HEAVY = 2   // Full tracking with details
    };

    struct AllocationInfo {
        void* address;
        size_t size;
        std::string category;
        int frameAllocated;
        // Stack trace (only in HEAVY mode)
        // std::vector<std::string> stackTrace;
    };

    struct SystemMemoryStats {
        std::string systemName;
        size_t totalAllocated;
        size_t peakAllocated;
        int allocationCount;
        int deallocationCount;
        int activeAllocations;
    };

    struct MemorySummary {
        size_t totalAllocated;
        size_t peakAllocated;
        int totalAllocations;
        int totalDeallocations;
        int activeAllocations;
        int leakCount;
        size_t leakedBytes;
        std::vector<SystemMemoryStats> systemStats;
    };

    static AnitoMemoryProfiler* getInstance();
    static void initialize(const std::string& outputDir = "anito-debug/profiling", 
                           TrackingMode mode = TrackingMode::LIGHT);
    static void destroy();

    /**
     * Record an allocation
     * @param address Allocated memory address
     * @param size Size in bytes
     * @param category System category (e.g., "Renderer", "Physics", "Audio")
     */
    void recordAllocation(void* address, size_t size, const std::string& category);

    /**
     * Record a deallocation
     * @param address Deallocated memory address
     */
    void recordDeallocation(void* address);

    /**
     * Get memory statistics for a specific system
     */
    bool getSystemStats(const std::string& systemName, SystemMemoryStats& outStats) const;

    /**
     * Get overall memory summary
     */
    MemorySummary getMemorySummary() const;

    /**
     * Export memory profiling data to JSON
     */
    void exportToJSON(const std::string& filename);

    /**
     * Print memory summary to console
     */
    void printMemorySummary() const;

    /**
     * Detect and report memory leaks
     */
    void detectLeaks();

    /**
     * Set tracking mode
     */
    void setTrackingMode(TrackingMode mode) { m_trackingMode = mode; }
    TrackingMode getTrackingMode() const { return m_trackingMode; }

    /**
     * Clear all tracking data (useful for profiling specific sections)
     */
    void clear();

    /**
     * Mark the current frame (for tracking per-frame allocations)
     */
    void beginFrame();

private:
    AnitoMemoryProfiler();
    ~AnitoMemoryProfiler();
    AnitoMemoryProfiler(const AnitoMemoryProfiler&) = delete;
    AnitoMemoryProfiler& operator=(const AnitoMemoryProfiler&) = delete;

    void init(const std::string& outputDir, TrackingMode mode);
    void release();
    void ensureOutputDirectoryExists(const std::string& path);
    void updateSystemStats(const std::string& category, size_t size, bool isAllocation);

    // Internal helper that assumes mutex is already locked
    MemorySummary _getMemorySummaryUnsafe() const;

    // Internal helper that assumes mutex is already locked
    void _detectLeaksUnsafe();

    static AnitoMemoryProfiler* s_instance;

    std::string m_outputDir;
    TrackingMode m_trackingMode;
    mutable std::mutex m_mutex;

    // Allocation tracking (HEAVY mode only)
    std::unordered_map<void*, AllocationInfo> m_allocations;

    // Per-system statistics (LIGHT and HEAVY modes)
    std::unordered_map<std::string, SystemMemoryStats> m_systemStats;

    // Global statistics
    size_t m_totalAllocated;
    size_t m_peakAllocated;
    int m_totalAllocations;
    int m_totalDeallocations;
    int m_currentFrame;
};

} // namespace Anito

// Memory tracking macros (can be disabled in Release builds)
#ifdef ANITO_ENABLE_MEMORY_PROFILING
    #define ANITO_TRACK_ALLOC(ptr, size, category) \
        if (Anito::AnitoMemoryProfiler::getInstance()) \
            Anito::AnitoMemoryProfiler::getInstance()->recordAllocation(ptr, size, category)

    #define ANITO_TRACK_DEALLOC(ptr) \
        if (Anito::AnitoMemoryProfiler::getInstance()) \
            Anito::AnitoMemoryProfiler::getInstance()->recordDeallocation(ptr)
#else
    #define ANITO_TRACK_ALLOC(ptr, size, category)
    #define ANITO_TRACK_DEALLOC(ptr)
#endif
