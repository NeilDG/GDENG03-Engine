#pragma once
// Performance Benchmarking System
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <vector>
#include <cstdint>

namespace Anito {

class AnitoEngine;  // Forward declaration

/**
 * BenchmarkDuration - Easy switching for developers
 */
enum class BenchmarkDuration {
    Quick,      // 30 seconds per resolution (~90s total)
    Standard,   // 60 seconds per resolution (~180s total)
    Extended    // 120 seconds per resolution (~240s total)
};

/**
 * Resolution configuration
 */
struct Resolution {
    uint32_t width;
    uint32_t height;

    std::string toString() const;
};

/**
 * BenchmarkConfig - Comprehensive benchmark configuration
 */
struct BenchmarkConfig {
    BenchmarkDuration duration = BenchmarkDuration::Quick;
    std::string buildConfig;  // "Debug" or "Release"
    std::string targetGPU;    // e.g., "RTX 4060 Ti 16GB"

    std::vector<Resolution> testResolutions = {
        {1280, 720},   // 720p
        {1920, 1080},  // 1080p
        {2560, 1440}   // 1440p
    };

    bool captureFrames = true;  // Capture comparison frames
    std::string outputDir = "anito-debug/benchmarks";

    // Get duration in seconds based on enum
    float getDurationSeconds() const;
};

/**
 * BenchmarkResults - Results for a single test configuration
 */
struct BenchmarkResults {
    std::string resolution;      // "1920x1080"
    std::string renderMode;      // "Forward" or "Deferred"
    std::string buildConfig;     // "Debug" or "Release"

    // FPS Statistics
    double avgFPS;
    double minFPS;
    double maxFPS;

    // Frame Time Statistics
    double avgFrameTimeMs;
    double minFrameTimeMs;
    double maxFrameTimeMs;
    double percentile95Ms;       // 95th percentile frame time
    double percentile99Ms;       // 99th percentile frame time

    // Memory Statistics
    uint64_t gpuMemoryUsedMB;
    uint64_t systemMemoryUsedMB;

    // Frame Statistics
    int totalFrames;
    int droppedFrames;           // Frames >33ms (60 FPS target)
    int framesUnder16ms;         // Frames meeting 60 FPS target

    // Pass/Fail
    bool meetsPerformanceTarget; // >60 FPS average
};

/**
 * AnitoBenchmarkTest - Automated performance benchmarking
 * 
 * Features:
 * - Multi-resolution testing (720p, 1080p, 1440p)
 * - Multi-configuration testing (Debug, Release)
 * - Forward vs Deferred comparison
 * - Automated frame capture at key moments
 * - Comprehensive JSON/CSV export
 * - Integration with existing profiling system
 * 
 * Professional References:
 * - SIGGRAPH Real-Time Rendering Course Notes (https://advances.realtimerendering.com/)
 * - NVIDIA Nsight Graphics Profiling Guide
 * - AMD GPU Performance Guidelines
 * - bgfx Performance Best Practices
 * 
 * Usage:
 *   BenchmarkConfig config;
 *   config.duration = BenchmarkDuration::Quick;
 *   AnitoBenchmarkTest benchmark;
 *   benchmark.runBenchmark(config, engine);
 */
class AnitoBenchmarkTest {
public:
    AnitoBenchmarkTest() = default;

    /**
     * Run complete benchmark suite with pre-initialized engine
     * @param config Benchmark configuration
     * @param engine Pointer to initialized AnitoEngine
     * @return Vector of benchmark results
     */
    std::vector<BenchmarkResults> runBenchmark(const BenchmarkConfig& config, AnitoEngine* engine);

    /**
     * Export all results to JSON
     * @param results Vector of all benchmark results
     * @param outputPath Output file path
     */
    void exportToJSON(
        const std::vector<BenchmarkResults>& results,
        const std::string& outputPath
    );

    /**
     * Export all results to CSV
     * @param results Vector of all benchmark results
     * @param outputPath Output file path
     */
    void exportToCSV(
        const std::vector<BenchmarkResults>& results,
        const std::string& outputPath
    );

    /**
     * Generate comparison report (console output)
     * @param results Vector of all benchmark results
     */
    void printComparisonReport(
        const std::vector<BenchmarkResults>& results
    );

private:
    BenchmarkResults runSingleTest(
        AnitoEngine* engine,
        const Resolution& resolution,
        float duration
    );

    void captureComparisonFrame(
        AnitoEngine* engine,
        const std::string& resolution,
        const std::string& renderMode,
        float timestamp
    );

    void ensureOutputDirectory(const std::string& path);
    std::string getCurrentTimestamp();
};

} // namespace Anito
