#include "AnitoMemoryProfiler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace Anito {

AnitoMemoryProfiler* AnitoMemoryProfiler::s_instance = nullptr;

AnitoMemoryProfiler* AnitoMemoryProfiler::getInstance() {
    return s_instance;
}

void AnitoMemoryProfiler::initialize(const std::string& outputDir, TrackingMode mode) {
    if (s_instance) {
        std::cerr << "[MemoryProfiler] Already initialized!" << std::endl;
        return;
    }

    s_instance = new AnitoMemoryProfiler();
    s_instance->init(outputDir, mode);
}

void AnitoMemoryProfiler::destroy() {
    if (s_instance) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoMemoryProfiler::AnitoMemoryProfiler()
    : m_outputDir("anito-debug/profiling")
    , m_trackingMode(TrackingMode::LIGHT)
    , m_totalAllocated(0)
    , m_peakAllocated(0)
    , m_totalAllocations(0)
    , m_totalDeallocations(0)
    , m_currentFrame(0)
{
}

AnitoMemoryProfiler::~AnitoMemoryProfiler() {
    release();
}

void AnitoMemoryProfiler::init(const std::string& outputDir, TrackingMode mode) {
    m_outputDir = outputDir;
    m_trackingMode = mode;
    ensureOutputDirectoryExists(m_outputDir);

    std::cout << "[MemoryProfiler] Initialized" << std::endl;
    std::cout << "  Output Dir: " << m_outputDir << std::endl;
    std::cout << "  Tracking Mode: ";

    switch (m_trackingMode) {
        case TrackingMode::NONE:
            std::cout << "NONE (No tracking)" << std::endl;
            break;
        case TrackingMode::LIGHT:
            std::cout << "LIGHT (Basic statistics)" << std::endl;
            break;
        case TrackingMode::HEAVY:
            std::cout << "HEAVY (Full tracking)" << std::endl;
            break;
    }
}

void AnitoMemoryProfiler::release() {
    // Export final memory profile before acquiring the lock to avoid recursive locking
    if (m_totalAllocations > 0) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);

        std::ostringstream oss;
        oss << "memory_profile_" << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S") << ".json";

        exportToJSON(m_outputDir + "/" + oss.str());
        std::cout << "[MemoryProfiler] Final profile exported: " << oss.str() << std::endl;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Detect leaks before shutdown (using unsafe version since we already hold the lock)
    _detectLeaksUnsafe();
}

void AnitoMemoryProfiler::recordAllocation(void* address, size_t size, const std::string& category) {
    if (m_trackingMode == TrackingMode::NONE || !address) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_totalAllocated += size;
    m_peakAllocated = std::max(m_peakAllocated, m_totalAllocated);
    m_totalAllocations++;

    // Update system stats
    updateSystemStats(category, size, true);

    // In HEAVY mode, track individual allocations
    if (m_trackingMode == TrackingMode::HEAVY) {
        AllocationInfo info;
        info.address = address;
        info.size = size;
        info.category = category;
        info.frameAllocated = m_currentFrame;

        m_allocations[address] = info;
    }
}

void AnitoMemoryProfiler::recordDeallocation(void* address) {
    if (m_trackingMode == TrackingMode::NONE || !address) return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_totalDeallocations++;

    // In HEAVY mode, find and remove the allocation
    if (m_trackingMode == TrackingMode::HEAVY) {
        auto it = m_allocations.find(address);
        if (it != m_allocations.end()) {
            const auto& info = it->second;
            m_totalAllocated -= info.size;

            // Update system stats
            updateSystemStats(info.category, info.size, false);

            m_allocations.erase(it);
        }
        else {
            // Deallocation without matching allocation (could be allocated before profiling started)
            // This is not necessarily an error
        }
    }
}

void AnitoMemoryProfiler::updateSystemStats(const std::string& category, size_t size, bool isAllocation) {
    auto& stats = m_systemStats[category];
    stats.systemName = category;

    if (isAllocation) {
        stats.totalAllocated += size;
        stats.peakAllocated = std::max(stats.peakAllocated, stats.totalAllocated);
        stats.allocationCount++;
        stats.activeAllocations++;
    }
    else {
        stats.totalAllocated -= size;
        stats.deallocationCount++;
        stats.activeAllocations--;
    }
}

bool AnitoMemoryProfiler::getSystemStats(const std::string& systemName, SystemMemoryStats& outStats) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_systemStats.find(systemName);
    if (it != m_systemStats.end()) {
        outStats = it->second;
        return true;
    }
    return false;
}

AnitoMemoryProfiler::MemorySummary AnitoMemoryProfiler::_getMemorySummaryUnsafe() const {
    // This method assumes the caller already holds m_mutex
    MemorySummary summary;
    summary.totalAllocated = m_totalAllocated;
    summary.peakAllocated = m_peakAllocated;
    summary.totalAllocations = m_totalAllocations;
    summary.totalDeallocations = m_totalDeallocations;
    summary.activeAllocations = m_totalAllocations - m_totalDeallocations;

    // Count leaks
    if (m_trackingMode == TrackingMode::HEAVY) {
        summary.leakCount = static_cast<int>(m_allocations.size());
        summary.leakedBytes = 0;
        for (const auto& [addr, info] : m_allocations) {
            summary.leakedBytes += info.size;
        }
    }
    else {
        summary.leakCount = 0;
        summary.leakedBytes = 0;
    }

    // Collect system stats
    for (const auto& [name, stats] : m_systemStats) {
        summary.systemStats.push_back(stats);
    }

    // Sort by total allocated
    std::sort(summary.systemStats.begin(), summary.systemStats.end(),
        [](const SystemMemoryStats& a, const SystemMemoryStats& b) {
            return a.totalAllocated > b.totalAllocated;
        });

    return summary;
}

AnitoMemoryProfiler::MemorySummary AnitoMemoryProfiler::getMemorySummary() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return _getMemorySummaryUnsafe();
}

void AnitoMemoryProfiler::exportToJSON(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[MemoryProfiler] Failed to open file for export: " << filename << std::endl;
        return;
    }

    auto summary = _getMemorySummaryUnsafe();

    file << "{\n";
    file << "  \"tracking_mode\": ";
    switch (m_trackingMode) {
        case TrackingMode::NONE:  file << "\"NONE\""; break;
        case TrackingMode::LIGHT: file << "\"LIGHT\""; break;
        case TrackingMode::HEAVY: file << "\"HEAVY\""; break;
    }
    file << ",\n";

    file << "  \"total_allocated_bytes\": " << summary.totalAllocated << ",\n";
    file << "  \"total_allocated_mb\": " << (summary.totalAllocated / (1024.0 * 1024.0)) << ",\n";
    file << "  \"peak_allocated_bytes\": " << summary.peakAllocated << ",\n";
    file << "  \"peak_allocated_mb\": " << (summary.peakAllocated / (1024.0 * 1024.0)) << ",\n";
    file << "  \"total_allocations\": " << summary.totalAllocations << ",\n";
    file << "  \"total_deallocations\": " << summary.totalDeallocations << ",\n";
    file << "  \"active_allocations\": " << summary.activeAllocations << ",\n";
    file << "  \"leak_count\": " << summary.leakCount << ",\n";
    file << "  \"leaked_bytes\": " << summary.leakedBytes << ",\n";
    file << "  \"systems\": [\n";

    for (size_t i = 0; i < summary.systemStats.size(); ++i) {
        const auto& stats = summary.systemStats[i];
        file << "    {\n";
        file << "      \"name\": \"" << stats.systemName << "\",\n";
        file << "      \"total_allocated_bytes\": " << stats.totalAllocated << ",\n";
        file << "      \"total_allocated_mb\": " << (stats.totalAllocated / (1024.0 * 1024.0)) << ",\n";
        file << "      \"peak_allocated_bytes\": " << stats.peakAllocated << ",\n";
        file << "      \"peak_allocated_mb\": " << (stats.peakAllocated / (1024.0 * 1024.0)) << ",\n";
        file << "      \"allocation_count\": " << stats.allocationCount << ",\n";
        file << "      \"deallocation_count\": " << stats.deallocationCount << ",\n";
        file << "      \"active_allocations\": " << stats.activeAllocations << "\n";
        file << "    }" << (i < summary.systemStats.size() - 1 ? ",\n" : "\n");
    }

    file << "  ]\n";
    file << "}\n";

    file.close();
    std::cout << "[MemoryProfiler] Exported to JSON: " << filename << std::endl;
}

void AnitoMemoryProfiler::printMemorySummary() const {
    auto summary = getMemorySummary();

    std::cout << "=== Memory Profile ===" << std::endl;
    std::cout << "Total Allocated: " << std::fixed << std::setprecision(2) 
              << (summary.totalAllocated / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "Peak Usage:      " << std::fixed << std::setprecision(2)
              << (summary.peakAllocated / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "Active Allocations: " << summary.activeAllocations << std::endl;

    if (m_trackingMode == TrackingMode::HEAVY && summary.leakCount > 0) {
        std::cout << "\n[WARNING] Leaks Detected: " << summary.leakCount 
                  << " allocations (" << (summary.leakedBytes / 1024.0) << " KB not freed)" << std::endl;
    }

    std::cout << "\nBy System:" << std::endl;
    for (const auto& stats : summary.systemStats) {
        double mb = stats.totalAllocated / (1024.0 * 1024.0);
        double percentage = (summary.totalAllocated > 0) 
            ? (stats.totalAllocated * 100.0 / summary.totalAllocated) 
            : 0.0;

        std::cout << "  " << std::setw(15) << std::left << stats.systemName << ": "
                  << std::setw(8) << std::right << std::fixed << std::setprecision(2) << mb << " MB ("
                  << std::setw(5) << std::fixed << std::setprecision(1) << percentage << "%) - "
                  << stats.activeAllocations << " allocations" << std::endl;
    }
}

void AnitoMemoryProfiler::_detectLeaksUnsafe() {
    // This method assumes the caller already holds m_mutex
    if (m_trackingMode != TrackingMode::HEAVY) {
        std::cout << "[MemoryProfiler] Leak detection only available in HEAVY tracking mode" << std::endl;
        return;
    }

    if (m_allocations.empty()) {
        std::cout << "[MemoryProfiler] No memory leaks detected!" << std::endl;
        return;
    }

    std::cout << "[MemoryProfiler] ================================" << std::endl;
    std::cout << "[MemoryProfiler] MEMORY LEAKS DETECTED!" << std::endl;
    std::cout << "[MemoryProfiler] ================================" << std::endl;
    std::cout << "[MemoryProfiler] Leaked Allocations: " << m_allocations.size() << std::endl;

    size_t totalLeaked = 0;
    std::unordered_map<std::string, size_t> leaksByCategory;

    for (const auto& [addr, info] : m_allocations) {
        totalLeaked += info.size;
        leaksByCategory[info.category] += info.size;
    }

    std::cout << "[MemoryProfiler] Total Leaked: " << (totalLeaked / 1024.0) << " KB" << std::endl;
    std::cout << "[MemoryProfiler] By Category:" << std::endl;

    for (const auto& [category, size] : leaksByCategory) {
        std::cout << "[MemoryProfiler]   " << category << ": " 
                  << (size / 1024.0) << " KB" << std::endl;
    }
}

void AnitoMemoryProfiler::detectLeaks() {
    std::lock_guard<std::mutex> lock(m_mutex);
    _detectLeaksUnsafe();
}

void AnitoMemoryProfiler::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_allocations.clear();
    m_systemStats.clear();
    m_totalAllocated = 0;
    m_peakAllocated = 0;
    m_totalAllocations = 0;
    m_totalDeallocations = 0;
}

void AnitoMemoryProfiler::beginFrame() {
    m_currentFrame++;
}

void AnitoMemoryProfiler::ensureOutputDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[MemoryProfiler] Failed to create output directory: " << e.what() << std::endl;
    }
}

} // namespace Anito
