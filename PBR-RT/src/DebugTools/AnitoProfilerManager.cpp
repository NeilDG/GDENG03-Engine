#include "AnitoProfilerManager.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm> // for std::min

namespace Anito {

bool AnitoProfilerManager::s_initialized = false;
std::string AnitoProfilerManager::s_outputDir = "anito-debug";

void AnitoProfilerManager::initialize(
    const std::string& outputDir,
    bool enableCPUProfiling,
    bool enableGPUProfiling,
    AnitoMemoryProfiler::TrackingMode memoryTrackingMode,
    bool enableCrashReporter
) {
    if (s_initialized) {
        std::cerr << "[ProfilerManager] Already initialized!" << std::endl;
        return;
    }

    s_outputDir = outputDir;

    std::cout << "========================================" << std::endl;
    std::cout << "Anito Profiler Manager - Initializing" << std::endl;
    std::cout << "========================================" << std::endl;

    // Initialize Console Logger first (so other systems can log)
    AnitoConsoleLogger::initialize(outputDir + "/logs");

    // Log initialization
    ANITO_LOG_INFO("ProfilerManager", "Initializing profiling systems...");

    // Initialize Crash Reporter early (to catch crashes during initialization)
    if (enableCrashReporter) {
        AnitoCrashReporter::initialize(outputDir + "/crashes");
        ANITO_LOG_INFO("ProfilerManager", "Crash Reporter initialized");
    }

    // Initialize Performance Profiler
    if (enableCPUProfiling) {
        AnitoPerformanceProfiler::initialize(outputDir + "/profiling");
        ANITO_LOG_INFO("ProfilerManager", "CPU Performance Profiler initialized");
    }

    // Initialize GPU Profiler
    if (enableGPUProfiling) {
        AnitoGPUProfiler::initialize(outputDir + "/profiling");
        ANITO_LOG_INFO("ProfilerManager", "GPU Profiler initialized");
    }

    // Initialize Memory Profiler
    if (memoryTrackingMode != AnitoMemoryProfiler::TrackingMode::NONE) {
        AnitoMemoryProfiler::initialize(outputDir + "/profiling", memoryTrackingMode);
        ANITO_LOG_INFO("ProfilerManager", "Memory Profiler initialized");
    }

    s_initialized = true;

    ANITO_LOG_INFO("ProfilerManager", "All profiling systems initialized successfully");
    std::cout << "========================================" << std::endl;
}

void AnitoProfilerManager::destroy() {
    if (!s_initialized) {
        return;
    }

    ANITO_LOG_INFO("ProfilerManager", "Shutting down profiling systems...");

    std::cout << "\n========================================" << std::endl;
    std::cout << "Anito Profiler Manager - Shutdown" << std::endl;
    std::cout << "========================================" << std::endl;

    // Print final summary
    printPerformanceSummary();

    // Export final profiles
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    std::ostringstream oss;
    oss << "final_session_" << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");
    exportAllProfiles(oss.str());

    // Destroy profilers in reverse order
    AnitoMemoryProfiler::destroy();
    AnitoGPUProfiler::destroy();
    AnitoPerformanceProfiler::destroy();
    AnitoCrashReporter::destroy();

    // Destroy logger last (so other systems can log during shutdown)
    if (AnitoConsoleLogger::getInstance()) {
        AnitoConsoleLogger::getInstance()->flush();
    }
    AnitoConsoleLogger::destroy();

    s_initialized = false;

    std::cout << "========================================" << std::endl;
}

void AnitoProfilerManager::beginFrame() {
    if (!s_initialized) return;

    // Begin CPU profiling
    if (AnitoPerformanceProfiler::getInstance()) {
        AnitoPerformanceProfiler::getInstance()->beginFrame();
    }

    // Begin GPU profiling
    if (AnitoGPUProfiler::getInstance()) {
        AnitoGPUProfiler::getInstance()->beginFrame();
    }

    // Mark frame for memory profiler
    if (AnitoMemoryProfiler::getInstance()) {
        AnitoMemoryProfiler::getInstance()->beginFrame();
    }
}

void AnitoProfilerManager::endFrame() {
    if (!s_initialized) return;

    // End GPU profiling
    if (AnitoGPUProfiler::getInstance()) {
        AnitoGPUProfiler::getInstance()->endFrame();
    }

    // End CPU profiling
    if (AnitoPerformanceProfiler::getInstance()) {
        AnitoPerformanceProfiler::getInstance()->endFrame();
    }
}

void AnitoProfilerManager::printPerformanceSummary() {
    if (!s_initialized) return;

    std::cout << "\n========================================" << std::endl;
    std::cout << "Performance Summary" << std::endl;
    std::cout << "========================================" << std::endl;

    // CPU Performance
    if (AnitoPerformanceProfiler::getInstance()) {
        auto profiler = AnitoPerformanceProfiler::getInstance();
        std::cout << "\n--- CPU Performance ---" << std::endl;
        std::cout << "Total Frames:   " << profiler->getTotalFrames() << std::endl;
        std::cout << "Average FPS:    " << std::fixed << std::setprecision(2) << profiler->getAverageFPS() << std::endl;
        std::cout << "Avg Frame Time: " << std::fixed << std::setprecision(2) << profiler->getAverageFrameTime() << " ms" << std::endl;
        std::cout << "Min Frame Time: " << std::fixed << std::setprecision(2) << profiler->getMinFrameTime() << " ms" << std::endl;
        std::cout << "Max Frame Time: " << std::fixed << std::setprecision(2) << profiler->getMaxFrameTime() << " ms" << std::endl;

        // Top 5 slowest systems
        auto stats = profiler->getAllStats();
        if (!stats.empty()) {
            std::cout << "\nTop 5 Slowest Systems:" << std::endl;
            size_t maxCount = (std::min)(stats.size(), size_t(5));
            for (size_t i = 0; i < maxCount; ++i) {
                std::cout << "  " << (i + 1) << ". " << std::setw(20) << std::left << stats[i].name
                          << ": " << std::setw(8) << std::right << std::fixed << std::setprecision(2) 
                          << stats[i].avgMs << " ms (" 
                          << std::fixed << std::setprecision(1) << stats[i].percentageOfFrame << "%)" << std::endl;
            }
        }
    }

    // GPU Performance
    if (AnitoGPUProfiler::getInstance()) {
        auto profiler = AnitoGPUProfiler::getInstance();
        std::cout << "\n--- GPU Performance ---" << std::endl;
        const auto& gpuStats = profiler->getLastFrameStats();
        std::cout << "Draw Calls:     " << gpuStats.totalDrawCalls << std::endl;
        std::cout << "Triangles:      " << gpuStats.totalTriangles << std::endl;
        std::cout << "Vertices:       " << gpuStats.totalVertices << std::endl;
        std::cout << "Texture Memory: " << gpuStats.textureMemoryMB << " MB" << std::endl;
        std::cout << "Buffer Memory:  " << gpuStats.bufferMemoryMB << " MB" << std::endl;
    }

    // Memory Usage
    if (AnitoMemoryProfiler::getInstance()) {
        auto profiler = AnitoMemoryProfiler::getInstance();
        std::cout << "\n--- Memory Usage ---" << std::endl;
        profiler->printMemorySummary();
    }

    std::cout << "========================================" << std::endl;
}

void AnitoProfilerManager::exportAllProfiles(const std::string& sessionName) {
    if (!s_initialized) return;

    ANITO_LOG_INFO("ProfilerManager", "Exporting all profiling data for session: " + sessionName);

    // Calculate session duration
    auto now = std::chrono::system_clock::now();
    static auto s_startTime = now;
    double sessionDuration = std::chrono::duration<double>(now - s_startTime).count();

    // Export CPU profile
    if (AnitoPerformanceProfiler::getInstance()) {
        std::string filename = s_outputDir + "/profiling/cpu_" + sessionName + ".json";
        AnitoPerformanceProfiler::getInstance()->exportToJSON(filename, sessionDuration);
    }

    // Export GPU profile
    if (AnitoGPUProfiler::getInstance()) {
        std::string jsonFilename = s_outputDir + "/profiling/gpu_" + sessionName + ".json";
        std::string csvFilename = s_outputDir + "/profiling/gpu_" + sessionName + ".csv";
        AnitoGPUProfiler::getInstance()->exportToJSON(jsonFilename);
        AnitoGPUProfiler::getInstance()->exportToCSV(csvFilename);
    }

    // Export Memory profile
    if (AnitoMemoryProfiler::getInstance()) {
        std::string filename = s_outputDir + "/profiling/memory_" + sessionName + ".json";
        AnitoMemoryProfiler::getInstance()->exportToJSON(filename);
    }

    ANITO_LOG_INFO("ProfilerManager", "All profiles exported successfully");
}

} // namespace Anito
