#pragma once
// Profiler Manager - Orchestrates All Profiling Systems
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include "AnitoConsoleLogger.h"
#include "AnitoPerformanceProfiler.h"
#include "AnitoGPUProfiler.h"
#include "AnitoMemoryProfiler.h"
#include "AnitoCrashReporter.h"
#include <string>

namespace Anito {

/**
 * AnitoProfilerManager - Central manager for all profiling systems
 * 
 * Features:
 * - Unified initialization/shutdown for all profilers
 * - Global enable/disable switches
 * - Automatic profiling data export on shutdown
 * - Frame synchronization across all profilers
 * 
 * Usage:
 *   AnitoProfilerManager::initialize();
 *   
 *   // In game loop:
 *   AnitoProfilerManager::beginFrame();
 *   // ... game code ...
 *   AnitoProfilerManager::endFrame();
 *   
 *   AnitoProfilerManager::destroy();
 */
class AnitoProfilerManager {
public:
    /**
     * Initialize all profiling systems
     * @param outputDir Base directory for all profiling output
     * @param enableCPUProfiling Enable CPU performance profiling
     * @param enableGPUProfiling Enable GPU performance profiling
     * @param memoryTrackingMode Memory tracking mode (NONE, LIGHT, HEAVY)
     * @param enableCrashReporter Enable crash reporting
     */
    static void initialize(
        const std::string& outputDir = "anito-debug",
        bool enableCPUProfiling = true,
        bool enableGPUProfiling = true,
        AnitoMemoryProfiler::TrackingMode memoryTrackingMode = AnitoMemoryProfiler::TrackingMode::LIGHT,
        bool enableCrashReporter = true
    );

    /**
     * Shutdown all profiling systems
     */
    static void destroy();

    /**
     * Begin frame profiling (call at start of frame)
     */
    static void beginFrame();

    /**
     * End frame profiling (call at end of frame)
     */
    static void endFrame();

    /**
     * Get individual profiler instances
     */
    static AnitoConsoleLogger* getLogger() { return AnitoConsoleLogger::getInstance(); }
    static AnitoPerformanceProfiler* getCPUProfiler() { return AnitoPerformanceProfiler::getInstance(); }
    static AnitoGPUProfiler* getGPUProfiler() { return AnitoGPUProfiler::getInstance(); }
    static AnitoMemoryProfiler* getMemoryProfiler() { return AnitoMemoryProfiler::getInstance(); }
    static AnitoCrashReporter* getCrashReporter() { return AnitoCrashReporter::getInstance(); }

    /**
     * Print performance summary to console
     */
    static void printPerformanceSummary();

    /**
     * Export all profiling data
     */
    static void exportAllProfiles(const std::string& sessionName = "session");

private:
    AnitoProfilerManager() = delete;
    ~AnitoProfilerManager() = delete;

    static bool s_initialized;
    static std::string s_outputDir;
};

} // namespace Anito
