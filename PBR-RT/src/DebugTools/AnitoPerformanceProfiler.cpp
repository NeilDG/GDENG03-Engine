#include "AnitoPerformanceProfiler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Anito {

AnitoPerformanceProfiler* AnitoPerformanceProfiler::s_instance = nullptr;

AnitoPerformanceProfiler* AnitoPerformanceProfiler::getInstance() {
    return s_instance;
}

void AnitoPerformanceProfiler::initialize(const std::string& outputDir) {
    if (s_instance) {
        std::cerr << "[PerformanceProfiler] Already initialized!" << std::endl;
        return;
    }

    s_instance = new AnitoPerformanceProfiler();
    s_instance->init(outputDir);
}

void AnitoPerformanceProfiler::destroy() {
    if (s_instance) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoPerformanceProfiler::AnitoPerformanceProfiler()
    : m_outputDir("anito-debug/profiling")
    , m_currentDepth(0)
    , m_currentFrameTime(0.0)
    , m_avgFrameTime(0.0)
    , m_minFrameTime(999999.0)
    , m_maxFrameTime(0.0)
    , m_avgFPS(0.0)
    , m_totalFrames(0)
    , m_enabled(true)
    , m_inFrame(false)
{
}

AnitoPerformanceProfiler::~AnitoPerformanceProfiler() {
    release();
}

void AnitoPerformanceProfiler::init(const std::string& outputDir) {
    m_outputDir = outputDir;
    ensureOutputDirectoryExists(m_outputDir);

    m_sessionStartTime = std::chrono::high_resolution_clock::now();
    m_frameHistory.reserve(MAX_FRAME_HISTORY);

    std::cout << "[PerformanceProfiler] Initialized" << std::endl;
    std::cout << "  Output Dir: " << m_outputDir << std::endl;
}

void AnitoPerformanceProfiler::release() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Export final profiling data
    if (m_totalFrames > 0) {
        auto now = std::chrono::high_resolution_clock::now();
        double sessionDuration = std::chrono::duration<double>(now - m_sessionStartTime).count();

        auto sessionTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm timeinfo;
        localtime_s(&timeinfo, &sessionTime);

        std::ostringstream oss;
        oss << "cpu_profile_" << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S") << ".json";

        exportToJSON(m_outputDir + "/" + oss.str(), sessionDuration);

        std::cout << "[PerformanceProfiler] Final profile exported: " << oss.str() << std::endl;
    }
}

void AnitoPerformanceProfiler::beginScope(const std::string& name) {
    if (!m_enabled || !m_inFrame) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    ProfileScope scope;
    scope.name = name;
    scope.startTime = std::chrono::high_resolution_clock::now();
    scope.depth = m_currentDepth;
    scope.parentIndex = m_scopeStack.empty() ? -1 : m_scopeStack.top();

    int scopeIndex = static_cast<int>(m_currentFrameScopes.size());
    m_currentFrameScopes.push_back(scope);
    m_scopeStack.push(scopeIndex);

    m_currentDepth++;
}

void AnitoPerformanceProfiler::endScope() {
    if (!m_enabled || !m_inFrame || m_scopeStack.empty()) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    int scopeIndex = m_scopeStack.top();
    m_scopeStack.pop();
    m_currentDepth--;

    auto& scope = m_currentFrameScopes[scopeIndex];
    scope.endTime = std::chrono::high_resolution_clock::now();
    scope.durationMs = std::chrono::duration<double, std::milli>(scope.endTime - scope.startTime).count();

    // Update statistics
    auto& stats = m_stats[scope.name];
    stats.name = scope.name;
    stats.callCount++;
    stats.totalMs += scope.durationMs;
    stats.minMs = std::min(stats.minMs == 0.0 ? scope.durationMs : stats.minMs, scope.durationMs);
    stats.maxMs = std::max(stats.maxMs, scope.durationMs);
    stats.avgMs = stats.totalMs / stats.callCount;
}

void AnitoPerformanceProfiler::beginFrame() {
    if (!m_enabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_frameStartTime = std::chrono::high_resolution_clock::now();
    m_currentFrameScopes.clear();
    while (!m_scopeStack.empty()) m_scopeStack.pop();
    m_currentDepth = 0;
    m_inFrame = true;
}

void AnitoPerformanceProfiler::endFrame() {
    if (!m_enabled || !m_inFrame) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    auto frameEndTime = std::chrono::high_resolution_clock::now();
    m_currentFrameTime = std::chrono::duration<double, std::milli>(frameEndTime - m_frameStartTime).count();

    updateFrameStats();

    // Store frame history
    if (m_frameHistory.size() >= MAX_FRAME_HISTORY) {
        m_frameHistory.erase(m_frameHistory.begin());
    }
    m_frameHistory.push_back(m_currentFrameScopes);

    m_totalFrames++;
    m_inFrame = false;
}

void AnitoPerformanceProfiler::updateFrameStats() {
    // Update frame time statistics
    m_minFrameTime = std::min(m_minFrameTime, m_currentFrameTime);
    m_maxFrameTime = std::max(m_maxFrameTime, m_currentFrameTime);

    // Calculate rolling average
    double alpha = 0.1; // Smoothing factor
    if (m_totalFrames == 0) {
        m_avgFrameTime = m_currentFrameTime;
    }
    else {
        m_avgFrameTime = alpha * m_currentFrameTime + (1.0 - alpha) * m_avgFrameTime;
    }

    // Calculate FPS
    if (m_avgFrameTime > 0.0) {
        m_avgFPS = 1000.0 / m_avgFrameTime;
    }

    // Update percentage of frame for each scope
    if (m_currentFrameTime > 0.0) {
        for (auto& [name, stats] : m_stats) {
            stats.percentageOfFrame = (stats.avgMs / m_currentFrameTime) * 100.0;
        }
    }
}

void AnitoPerformanceProfiler::exportToJSON(const std::string& filename, double sessionDuration) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[PerformanceProfiler] Failed to open file for export: " << filename << std::endl;
        return;
    }

    file << "{\n";
    file << "  \"session\": \"" << filename << "\",\n";
    file << "  \"duration_seconds\": " << sessionDuration << ",\n";
    file << "  \"frames\": " << m_totalFrames << ",\n";
    file << "  \"avg_fps\": " << m_avgFPS << ",\n";
    file << "  \"frame_times\": {\n";
    file << "    \"avg_ms\": " << m_avgFrameTime << ",\n";
    file << "    \"min_ms\": " << m_minFrameTime << ",\n";
    file << "    \"max_ms\": " << m_maxFrameTime << "\n";
    file << "  },\n";
    file << "  \"systems\": [\n";

    // Sort by total time
    std::vector<ProfileStats> sortedStats;
    for (const auto& [name, stats] : m_stats) {
        sortedStats.push_back(stats);
    }
    std::sort(sortedStats.begin(), sortedStats.end(), 
        [](const ProfileStats& a, const ProfileStats& b) {
            return a.totalMs > b.totalMs;
        });

    for (size_t i = 0; i < sortedStats.size(); ++i) {
        const auto& stats = sortedStats[i];
        file << "    {\n";
        file << "      \"name\": \"" << stats.name << "\",\n";
        file << "      \"call_count\": " << stats.callCount << ",\n";
        file << "      \"total_ms\": " << stats.totalMs << ",\n";
        file << "      \"avg_ms\": " << stats.avgMs << ",\n";
        file << "      \"min_ms\": " << stats.minMs << ",\n";
        file << "      \"max_ms\": " << stats.maxMs << ",\n";
        file << "      \"percentage\": " << stats.percentageOfFrame << "\n";
        file << "    }" << (i < sortedStats.size() - 1 ? ",\n" : "\n");
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    std::cout << "[PerformanceProfiler] Exported to JSON: " << filename << std::endl;
}

void AnitoPerformanceProfiler::exportToChromeTracing(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[PerformanceProfiler] Failed to open file for Chrome Tracing export: " << filename << std::endl;
        return;
    }

    file << "[\n";

    bool first = true;
    int frameIndex = 0;

    for (const auto& frameScopes : m_frameHistory) {
        for (const auto& scope : frameScopes) {
            if (!first) file << ",\n";
            first = false;

            auto startUs = std::chrono::duration_cast<std::chrono::microseconds>(
                scope.startTime.time_since_epoch()).count();
            auto durationUs = scope.durationMs * 1000.0;

            file << "  {\n";
            file << "    \"name\": \"" << scope.name << "\",\n";
            file << "    \"cat\": \"function\",\n";
            file << "    \"ph\": \"X\",\n";
            file << "    \"ts\": " << startUs << ",\n";
            file << "    \"dur\": " << durationUs << ",\n";
            file << "    \"pid\": 1,\n";
            file << "    \"tid\": 1,\n";
            file << "    \"args\": { \"frame\": " << frameIndex << " }\n";
            file << "  }";
        }
        frameIndex++;
    }

    file << "\n]\n";
    file.close();

    std::cout << "[PerformanceProfiler] Exported to Chrome Tracing: " << filename << std::endl;
}

bool AnitoPerformanceProfiler::getStats(const std::string& scopeName, ProfileStats& outStats) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));

    auto it = m_stats.find(scopeName);
    if (it != m_stats.end()) {
        outStats = it->second;
        return true;
    }
    return false;
}

std::vector<AnitoPerformanceProfiler::ProfileStats> AnitoPerformanceProfiler::getAllStats() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_mutex));

    std::vector<ProfileStats> result;
    for (const auto& [name, stats] : m_stats) {
        result.push_back(stats);
    }

    // Sort by total time
    std::sort(result.begin(), result.end(), 
        [](const ProfileStats& a, const ProfileStats& b) {
            return a.totalMs > b.totalMs;
        });

    return result;
}

void AnitoPerformanceProfiler::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_currentFrameScopes.clear();
    while (!m_scopeStack.empty()) m_scopeStack.pop();
    m_stats.clear();
    m_frameHistory.clear();

    m_currentDepth = 0;
    m_currentFrameTime = 0.0;
    m_avgFrameTime = 0.0;
    m_minFrameTime = 999999.0;
    m_maxFrameTime = 0.0;
    m_avgFPS = 0.0;
    m_totalFrames = 0;
    m_inFrame = false;

    m_sessionStartTime = std::chrono::high_resolution_clock::now();
}

void AnitoPerformanceProfiler::ensureOutputDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[PerformanceProfiler] Failed to create output directory: " << e.what() << std::endl;
    }
}

} // namespace Anito
