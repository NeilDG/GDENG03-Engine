// Performance Benchmarking System Implementation
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include "AnitoBenchmarkTest.h"
#include "../AnitoEngine.h"
#include "../DebugTools/AnitoConsoleLogger.h"
#include "../DebugTools/AnitoProfilerManager.h"
#include "../DebugTools/AnitoPerformanceProfiler.h"
#include "../DebugTools/AnitoFrameCaptureRecorder.h"
#include "../Renderer/AnitoRenderer.h"
#include "../Window/AnitoWindow.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace Anito {

// Resolution toString
std::string Resolution::toString() const {
    return std::to_string(width) + "x" + std::to_string(height);
}

// BenchmarkConfig getDurationSeconds
float BenchmarkConfig::getDurationSeconds() const {
    switch (duration) {
        case BenchmarkDuration::Quick:    return 30.0f;
        case BenchmarkDuration::Standard: return 60.0f;
        case BenchmarkDuration::Extended: return 120.0f;
        default:                          return 30.0f;
    }
}

// Get current timestamp string
std::string AnitoBenchmarkTest::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

// Ensure output directory exists
void AnitoBenchmarkTest::ensureOutputDirectory(const std::string& path) {
    std::filesystem::create_directories(path);
}

// Run complete benchmark suite
std::vector<BenchmarkResults> AnitoBenchmarkTest::runBenchmark(const BenchmarkConfig& config, AnitoEngine* engine) {
    ANITO_LOG_INFO("Benchmark", "===========================================");
    ANITO_LOG_INFO("Benchmark", "  Anito Engine - Benchmark Suite");
    ANITO_LOG_INFO("Benchmark", "===========================================");

    std::string durationStr;
    switch (config.duration) {
        case BenchmarkDuration::Quick:    durationStr = "Quick (" + std::to_string((int)config.getDurationSeconds()) + "s)"; break;
        case BenchmarkDuration::Standard: durationStr = "Standard (" + std::to_string((int)config.getDurationSeconds()) + "s)"; break;
        case BenchmarkDuration::Extended: durationStr = "Extended (" + std::to_string((int)config.getDurationSeconds()) + "s)"; break;
    }

    ANITO_LOG_INFO("Benchmark", "Duration Mode: " + durationStr);
    ANITO_LOG_INFO("Benchmark", "Build Config: " + config.buildConfig);
    ANITO_LOG_INFO("Benchmark", "Target GPU: " + config.targetGPU);
    ANITO_LOG_INFO("Benchmark", "Resolutions: " + std::to_string(config.testResolutions.size()));
    ANITO_LOG_INFO("Benchmark", "===========================================");

    // Create output directory with timestamp
    std::string timestamp = getCurrentTimestamp();
    std::string sessionDir = config.outputDir + "/benchmark_" + timestamp;
    ensureOutputDirectory(sessionDir);

    ANITO_LOG_INFO("Benchmark", "Output Directory: " + sessionDir);

    std::vector<BenchmarkResults> allResults;

    // Run benchmarks for each resolution
    for (const auto& resolution : config.testResolutions) {
        ANITO_LOG_INFO("Benchmark", "");
        ANITO_LOG_INFO("Benchmark", "-------------------------------------------");
        ANITO_LOG_INFO("Benchmark", "Testing Resolution: " + resolution.toString());
        ANITO_LOG_INFO("Benchmark", "-------------------------------------------");

        // Run single test for this resolution
        BenchmarkResults results = runSingleTest(engine, resolution, config.getDurationSeconds());
        results.buildConfig = config.buildConfig;
        allResults.push_back(results);

        ANITO_LOG_INFO("Benchmark", "Results:");
        ANITO_LOG_INFO("Benchmark", "  Avg FPS: " + std::to_string(results.avgFPS));
        ANITO_LOG_INFO("Benchmark", "  Min FPS: " + std::to_string(results.minFPS));
        ANITO_LOG_INFO("Benchmark", "  Avg Frame Time: " + std::to_string(results.avgFrameTimeMs) + " ms");
    }

    ANITO_LOG_INFO("Benchmark", "");
    ANITO_LOG_INFO("Benchmark", "===========================================");
    ANITO_LOG_INFO("Benchmark", "  Benchmark Complete");
    ANITO_LOG_INFO("Benchmark", "===========================================");

    return allResults;
}

// Run single benchmark test
BenchmarkResults AnitoBenchmarkTest::runSingleTest(
    AnitoEngine* engine,
    const Resolution& resolution,
    float duration
) {
    BenchmarkResults results;
    results.resolution = resolution.toString();
    results.renderMode = "Deferred";  // Always deferred in benchmark mode

    ANITO_LOG_INFO("Benchmark", "Starting benchmark run for " + results.resolution);

    // Resize window to target resolution
    if (engine->getWindow()) {
        engine->getWindow()->resize(resolution.width, resolution.height);
        ANITO_LOG_INFO("Benchmark", "Window resized to " + results.resolution);
    }

    // Begin profiler benchmark session
    auto* profiler = AnitoProfilerManager::getCPUProfiler();
    if (profiler) {
        profiler->beginBenchmarkSession(results.resolution, results.renderMode, results.buildConfig);
        ANITO_LOG_INFO("Benchmark", "Profiler benchmark session started");
    }

    // Set up frame capture timestamps
    std::vector<float> captureTimestamps = { 0.0f, duration * 0.5f, duration - 1.0f };  // Start, middle, end
    size_t nextCaptureIndex = 0;
    float elapsed = 0.0f;
    float lastCaptureCheck = 0.0f;

    ANITO_LOG_INFO("Benchmark", "Running engine for " + std::to_string((int)duration) + " seconds...");
    ANITO_LOG_INFO("Benchmark", "Frames will be captured at: 0s, " + std::to_string((int)(duration * 0.5f)) + "s, " + std::to_string((int)(duration - 1.0f)) + "s");

    // Trigger frame captures during the run by checking elapsed time
    // The engine will run its normal loop and the profiler will collect data automatically
    // We just need to wait for the duration to complete

    // Note: Since we can't easily inject into the engine's run loop without major refactoring,
    // we'll rely on the runtime limit system. Set the max runtime to our benchmark duration.
    // The profiler's benchmark session is already active and will collect frame time samples.

    // For now, just sleep and let the engine run (not ideal, but works for this prototype)
    // In a production system, we would refactor the engine loop to be more testable

    ANITO_LOG_INFO("Benchmark", "NOTE: Benchmark will run for the duration set in the engine");
    ANITO_LOG_INFO("Benchmark", "Profiling data will be collected automatically by the profiler system");

    // End profiler benchmark session and get samples
    // Note: This happens immediately since we can't actually run the loop here
    // The real profiling happens during the engine's normal run() execution
    std::vector<double> frameTimeSamples;
    if (profiler) {
        // For now, end the session immediately
        // In reality, the session should stay active during engine->run()
        // and we would call endBenchmarkSession() after run() completes
        ANITO_LOG_WARNING("Benchmark", "Benchmark mode needs engine integration to work properly");
        ANITO_LOG_WARNING("Benchmark", "Current implementation is a prototype stub");
        profiler->endBenchmarkSession();
        frameTimeSamples = profiler->getFrameTimeSamples();
    }

    // Calculate statistics from frame time samples
    if (!frameTimeSamples.empty()) {
        // Sort for percentile calculations
        std::sort(frameTimeSamples.begin(), frameTimeSamples.end());

        // Calculate basic stats
        double sum = std::accumulate(frameTimeSamples.begin(), frameTimeSamples.end(), 0.0);
        results.avgFrameTimeMs = (sum / frameTimeSamples.size()) * 1000.0;  // Convert to ms
        results.minFrameTimeMs = frameTimeSamples.front() * 1000.0;
        results.maxFrameTimeMs = frameTimeSamples.back() * 1000.0;

        // Calculate FPS
        results.avgFPS = 1000.0 / results.avgFrameTimeMs;
        results.minFPS = 1000.0 / results.maxFrameTimeMs;
        results.maxFPS = 1000.0 / results.minFrameTimeMs;

        // Calculate percentiles
        size_t p95Index = static_cast<size_t>(frameTimeSamples.size() * 0.95);
        size_t p99Index = static_cast<size_t>(frameTimeSamples.size() * 0.99);
        results.percentile95Ms = frameTimeSamples[p95Index] * 1000.0;
        results.percentile99Ms = frameTimeSamples[p99Index] * 1000.0;

        // Frame statistics
        results.totalFrames = static_cast<int>(frameTimeSamples.size());
        results.framesUnder16ms = static_cast<int>(std::count_if(frameTimeSamples.begin(), frameTimeSamples.end(),
            [](double t) { return (t * 1000.0) < 16.67; }));
        results.droppedFrames = results.totalFrames - results.framesUnder16ms;

        // Performance target check (>60 FPS average)
        results.meetsPerformanceTarget = (results.avgFPS >= 60.0);
    } else {
        ANITO_LOG_WARNING("Benchmark", "No frame time samples collected - using placeholder values");
        results.avgFPS = 0.0;
        results.minFPS = 0.0;
        results.maxFPS = 0.0;
        results.avgFrameTimeMs = 0.0;
        results.minFrameTimeMs = 0.0;
        results.maxFrameTimeMs = 0.0;
        results.percentile95Ms = 0.0;
        results.percentile99Ms = 0.0;
        results.totalFrames = 0;
        results.droppedFrames = 0;
        results.framesUnder16ms = 0;
        results.meetsPerformanceTarget = false;
    }

    // Memory statistics (placeholder for now)
    results.gpuMemoryUsedMB = 0;  // TODO: Implement GPU memory tracking
    results.systemMemoryUsedMB = 0;  // TODO: Implement system memory tracking

    return results;
}

// Export to JSON
void AnitoBenchmarkTest::exportToJSON(
    const std::vector<BenchmarkResults>& results,
    const std::string& outputPath
) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        ANITO_LOG_ERROR("Benchmark", "Failed to open file for writing: " + outputPath);
        return;
    }

    file << "{\n";
    file << "  \"benchmark_session\": {\n";
    file << "    \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    file << "    \"total_tests\": " << results.size() << "\n";
    file << "  },\n";
    file << "  \"results\": [\n";

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];

        file << "    {\n";
        file << "      \"resolution\": \"" << r.resolution << "\",\n";
        file << "      \"renderMode\": \"" << r.renderMode << "\",\n";
        file << "      \"buildConfig\": \"" << r.buildConfig << "\",\n";
        file << "      \"avgFPS\": " << r.avgFPS << ",\n";
        file << "      \"minFPS\": " << r.minFPS << ",\n";
        file << "      \"maxFPS\": " << r.maxFPS << ",\n";
        file << "      \"avgFrameTimeMs\": " << r.avgFrameTimeMs << ",\n";
        file << "      \"minFrameTimeMs\": " << r.minFrameTimeMs << ",\n";
        file << "      \"maxFrameTimeMs\": " << r.maxFrameTimeMs << ",\n";
        file << "      \"percentile95Ms\": " << r.percentile95Ms << ",\n";
        file << "      \"percentile99Ms\": " << r.percentile99Ms << ",\n";
        file << "      \"gpuMemoryUsedMB\": " << r.gpuMemoryUsedMB << ",\n";
        file << "      \"systemMemoryUsedMB\": " << r.systemMemoryUsedMB << ",\n";
        file << "      \"totalFrames\": " << r.totalFrames << ",\n";
        file << "      \"droppedFrames\": " << r.droppedFrames << ",\n";
        file << "      \"framesUnder16ms\": " << r.framesUnder16ms << ",\n";
        file << "      \"meetsPerformanceTarget\": " << (r.meetsPerformanceTarget ? "true" : "false") << "\n";
        file << "    }";

        if (i < results.size() - 1) {
            file << ",";
        }
        file << "\n";
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    ANITO_LOG_INFO("Benchmark", "Exported JSON: " + outputPath);
}

// Export to CSV
void AnitoBenchmarkTest::exportToCSV(
    const std::vector<BenchmarkResults>& results,
    const std::string& outputPath
) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        ANITO_LOG_ERROR("Benchmark", "Failed to open file for writing: " + outputPath);
        return;
    }

    // Header
    file << "Resolution,RenderMode,BuildConfig,AvgFPS,MinFPS,MaxFPS,";
    file << "AvgFrameTimeMs,MinFrameTimeMs,MaxFrameTimeMs,";
    file << "Percentile95Ms,Percentile99Ms,";
    file << "GPUMemoryMB,SystemMemoryMB,";
    file << "TotalFrames,DroppedFrames,FramesUnder16ms,";
    file << "MeetsTarget\n";

    // Data rows
    for (const auto& r : results) {
        file << r.resolution << ","
             << r.renderMode << ","
             << r.buildConfig << ","
             << r.avgFPS << ","
             << r.minFPS << ","
             << r.maxFPS << ","
             << r.avgFrameTimeMs << ","
             << r.minFrameTimeMs << ","
             << r.maxFrameTimeMs << ","
             << r.percentile95Ms << ","
             << r.percentile99Ms << ","
             << r.gpuMemoryUsedMB << ","
             << r.systemMemoryUsedMB << ","
             << r.totalFrames << ","
             << r.droppedFrames << ","
             << r.framesUnder16ms << ","
             << (r.meetsPerformanceTarget ? "PASS" : "FAIL") << "\n";
    }

    file.close();
    ANITO_LOG_INFO("Benchmark", "Exported CSV: " + outputPath);
}

// Print comparison report
void AnitoBenchmarkTest::printComparisonReport(
    const std::vector<BenchmarkResults>& results
) {
    ANITO_LOG_INFO("Benchmark", "===========================================");
    ANITO_LOG_INFO("Benchmark", "  BENCHMARK RESULTS");
    ANITO_LOG_INFO("Benchmark", "===========================================");
    ANITO_LOG_INFO("Benchmark", "");
    ANITO_LOG_INFO("Benchmark", "Resolution | Mode     | Config  | Avg FPS | Min FPS | Frame Time | Status");
    ANITO_LOG_INFO("Benchmark", "-----------+----------+---------+---------+---------+------------+-------");

    for (const auto& r : results) {
        std::ostringstream line;
        line << std::setw(10) << std::left << r.resolution << " | "
             << std::setw(8) << std::left << r.renderMode << " | "
             << std::setw(7) << std::left << r.buildConfig << " | "
             << std::setw(7) << std::fixed << std::setprecision(1) << r.avgFPS << " | "
             << std::setw(7) << std::fixed << std::setprecision(1) << r.minFPS << " | "
             << std::setw(10) << std::fixed << std::setprecision(2) << r.avgFrameTimeMs << " ms | "
             << (r.meetsPerformanceTarget ? "PASS" : "FAIL");

        ANITO_LOG_INFO("Benchmark", line.str());
    }

    ANITO_LOG_INFO("Benchmark", "");
    ANITO_LOG_INFO("Benchmark", "===========================================");
    ANITO_LOG_INFO("Benchmark", "  PERFORMANCE TARGET VALIDATION");
    ANITO_LOG_INFO("Benchmark", "===========================================");

    // Find 1080p Release results
    for (const auto& r : results) {
        if (r.resolution == "1920x1080" && r.buildConfig == "Release") {
            std::string status = r.avgFPS > 60.0 ? "✓ PASSED" : "✗ FAILED";
            ANITO_LOG_INFO("Benchmark", r.renderMode + ": " + std::to_string(r.avgFPS) + " FPS - " + status);
        }
    }
}

// Capture comparison frame
void AnitoBenchmarkTest::captureComparisonFrame(
    AnitoEngine* engine,
    const std::string& resolution,
    const std::string& renderMode,
    float timestamp
) {
    ANITO_LOG_INFO("Benchmark", "Capturing frame: " + resolution + "_" + renderMode + "_" + std::to_string((int)timestamp) + "s");

    // Use existing frame capture recorder if available
    if (engine && engine->getFrameCaptureRecorder()) {
        // Manually trigger a frame capture
        // Note: The frame will be saved with the default naming scheme
        // To get custom names, we would need to extend AnitoFrameCaptureRecorder
        engine->getFrameCaptureRecorder()->captureFrame();
    } else {
        ANITO_LOG_WARNING("Benchmark", "Frame capture recorder not available");
    }
}

} // namespace Anito
