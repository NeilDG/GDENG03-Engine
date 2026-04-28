#include "AnitoGPUProfiler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace Anito {

AnitoGPUProfiler* AnitoGPUProfiler::s_instance = nullptr;

AnitoGPUProfiler* AnitoGPUProfiler::getInstance() {
    return s_instance;
}

void AnitoGPUProfiler::initialize(const std::string& outputDir) {
    if (s_instance) {
        std::cerr << "[GPUProfiler] Already initialized!" << std::endl;
        return;
    }

    s_instance = new AnitoGPUProfiler();
    s_instance->init(outputDir);
}

void AnitoGPUProfiler::destroy() {
    if (s_instance) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoGPUProfiler::AnitoGPUProfiler()
    : m_outputDir("anito-debug/profiling")
    , m_avgGPUTime(0.0)
    , m_totalFrames(0)
    , m_textureMemoryMB(0)
    , m_bufferMemoryMB(0)
    , m_enabled(true)
    , m_inFrame(false)
    , m_inPass(false)
{
}

AnitoGPUProfiler::~AnitoGPUProfiler() {
    release();
}

void AnitoGPUProfiler::init(const std::string& outputDir) {
    m_outputDir = outputDir;
    ensureOutputDirectoryExists(m_outputDir);

    m_frameHistory.reserve(MAX_FRAME_HISTORY);

    std::cout << "[GPUProfiler] Initialized" << std::endl;
    std::cout << "  Output Dir: " << m_outputDir << std::endl;
    std::cout << "  Note: GPU timing is estimated based on frame time" << std::endl;
    std::cout << "  For accurate GPU profiling, use NVIDIA Nsight or RenderDoc" << std::endl;
}

void AnitoGPUProfiler::release() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Export final profiling data
    if (m_totalFrames > 0) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);

        std::ostringstream oss;
        oss << "gpu_profile_" << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");

        exportToJSON(m_outputDir + "/" + oss.str() + ".json");
        exportToCSV(m_outputDir + "/" + oss.str() + ".csv");

        std::cout << "[GPUProfiler] Final profiles exported" << std::endl;
    }
}

void AnitoGPUProfiler::beginFrame() {
    if (!m_enabled) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_currentFrame = GPUFrameStats();
    m_currentFrame.totalGPUTimeMs = 0.0;
    m_currentFrame.totalDrawCalls = 0;
    m_currentFrame.totalTriangles = 0;
    m_currentFrame.totalVertices = 0;
    m_currentFrame.textureMemoryMB = m_textureMemoryMB;
    m_currentFrame.bufferMemoryMB = m_bufferMemoryMB;
    m_currentFrame.passes.clear();

    m_inFrame = true;
}

void AnitoGPUProfiler::endFrame() {
    if (!m_enabled || !m_inFrame) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_lastFrameStats = m_currentFrame;
    updateAverageGPUTime();

    // Store frame history
    if (m_frameHistory.size() >= MAX_FRAME_HISTORY) {
        m_frameHistory.erase(m_frameHistory.begin());
    }
    m_frameHistory.push_back(m_currentFrame);

    m_totalFrames++;
    m_inFrame = false;
}

void AnitoGPUProfiler::beginPass(const std::string& passName) {
    if (!m_enabled || !m_inFrame) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_inPass) {
        std::cerr << "[GPUProfiler] Warning: beginPass called without endPass!" << std::endl;
        endPass(); // Auto-end previous pass
    }

    m_currentPassName = passName;
    m_currentPass = GPUPassStats();
    m_currentPass.name = passName;
    m_currentPass.gpuTimeMs = 0.0;
    m_currentPass.drawCalls = 0;
    m_currentPass.triangles = 0;
    m_currentPass.vertices = 0;

    m_inPass = true;
}

void AnitoGPUProfiler::endPass() {
    if (!m_enabled || !m_inFrame || !m_inPass) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    // Note: We don't have actual GPU timing from bgfx
    // This is a placeholder for future implementation with platform-specific queries
    // For now, we track draw calls and geometry stats

    m_currentFrame.passes.push_back(m_currentPass);
    m_currentFrame.totalDrawCalls += m_currentPass.drawCalls;
    m_currentFrame.totalTriangles += m_currentPass.triangles;
    m_currentFrame.totalVertices += m_currentPass.vertices;

    m_inPass = false;
}

void AnitoGPUProfiler::recordDrawCall(int triangles, int vertices) {
    if (!m_enabled || !m_inFrame) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_inPass) {
        m_currentPass.drawCalls++;
        m_currentPass.triangles += triangles;
        m_currentPass.vertices += vertices;
    }
}

void AnitoGPUProfiler::updateAverageGPUTime() {
    // Calculate rolling average
    double alpha = 0.1; // Smoothing factor
    if (m_totalFrames == 0) {
        m_avgGPUTime = m_currentFrame.totalGPUTimeMs;
    }
    else {
        m_avgGPUTime = alpha * m_currentFrame.totalGPUTimeMs + (1.0 - alpha) * m_avgGPUTime;
    }
}

void AnitoGPUProfiler::exportToJSON(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[GPUProfiler] Failed to open file for export: " << filename << std::endl;
        return;
    }

    file << "{\n";
    file << "  \"total_frames\": " << m_totalFrames << ",\n";
    file << "  \"avg_gpu_time_ms\": " << m_avgGPUTime << ",\n";
    file << "  \"last_frame\": {\n";
    file << "    \"total_gpu_time_ms\": " << m_lastFrameStats.totalGPUTimeMs << ",\n";
    file << "    \"total_draw_calls\": " << m_lastFrameStats.totalDrawCalls << ",\n";
    file << "    \"total_triangles\": " << m_lastFrameStats.totalTriangles << ",\n";
    file << "    \"total_vertices\": " << m_lastFrameStats.totalVertices << ",\n";
    file << "    \"texture_memory_mb\": " << m_lastFrameStats.textureMemoryMB << ",\n";
    file << "    \"buffer_memory_mb\": " << m_lastFrameStats.bufferMemoryMB << ",\n";
    file << "    \"passes\": [\n";

    for (size_t i = 0; i < m_lastFrameStats.passes.size(); ++i) {
        const auto& pass = m_lastFrameStats.passes[i];
        file << "      {\n";
        file << "        \"name\": \"" << pass.name << "\",\n";
        file << "        \"gpu_time_ms\": " << pass.gpuTimeMs << ",\n";
        file << "        \"draw_calls\": " << pass.drawCalls << ",\n";
        file << "        \"triangles\": " << pass.triangles << ",\n";
        file << "        \"vertices\": " << pass.vertices << "\n";
        file << "      }" << (i < m_lastFrameStats.passes.size() - 1 ? ",\n" : "\n");
    }

    file << "    ]\n";
    file << "  },\n";
    file << "  \"frame_history\": [\n";

    for (size_t i = 0; i < m_frameHistory.size(); ++i) {
        const auto& frame = m_frameHistory[i];
        file << "    {\n";
        file << "      \"draw_calls\": " << frame.totalDrawCalls << ",\n";
        file << "      \"triangles\": " << frame.totalTriangles << ",\n";
        file << "      \"vertices\": " << frame.totalVertices << "\n";
        file << "    }" << (i < m_frameHistory.size() - 1 ? ",\n" : "\n");
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    std::cout << "[GPUProfiler] Exported to JSON: " << filename << std::endl;
}

void AnitoGPUProfiler::exportToCSV(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[GPUProfiler] Failed to open file for CSV export: " << filename << std::endl;
        return;
    }

    // Header
    file << "Frame,DrawCalls,Triangles,Vertices,TextureMemoryMB,BufferMemoryMB\n";

    // Data
    for (size_t i = 0; i < m_frameHistory.size(); ++i) {
        const auto& frame = m_frameHistory[i];
        file << i << ","
             << frame.totalDrawCalls << ","
             << frame.totalTriangles << ","
             << frame.totalVertices << ","
             << frame.textureMemoryMB << ","
             << frame.bufferMemoryMB << "\n";
    }

    file.close();
    std::cout << "[GPUProfiler] Exported to CSV: " << filename << std::endl;
}

void AnitoGPUProfiler::printFrameStats() const {
    std::cout << "=== GPU Profile (Frame " << m_totalFrames << ") ===" << std::endl;
    std::cout << "Total GPU Time: " << std::fixed << std::setprecision(2) 
              << m_lastFrameStats.totalGPUTimeMs << " ms" << std::endl;

    for (const auto& pass : m_lastFrameStats.passes) {
        std::cout << "  - " << std::setw(20) << std::left << pass.name << ": "
                  << std::setw(6) << std::right << pass.gpuTimeMs << " ms ("
                  << std::setw(4) << pass.drawCalls << " draw calls)" << std::endl;
    }

    std::cout << "\nDraw Calls:     " << m_lastFrameStats.totalDrawCalls << std::endl;
    std::cout << "Triangles:      " << m_lastFrameStats.totalTriangles << std::endl;
    std::cout << "Vertices:       " << m_lastFrameStats.totalVertices << std::endl;
    std::cout << "Texture Memory: " << m_lastFrameStats.textureMemoryMB << " MB" << std::endl;
    std::cout << "Buffer Memory:  " << m_lastFrameStats.bufferMemoryMB << " MB" << std::endl;
}

void AnitoGPUProfiler::ensureOutputDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[GPUProfiler] Failed to create output directory: " << e.what() << std::endl;
    }
}

} // namespace Anito
