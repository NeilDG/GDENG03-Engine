#pragma once
// Crash Reporter for Production Debugging
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include <string>
#include <functional>

// Define NOMINMAX before including Windows.h to avoid min/max macro conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

// Undefine near/far macros that conflict with camera parameters
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

namespace Anito {

/**
 * AnitoCrashReporter - Capture crashes and generate debug information
 * 
 * Features:
 * - Exception handling and stack trace capture
 * - Minidump generation (Windows)
 * - Last N log entries before crash
 * - System information (GPU, CPU, OS, memory)
 * - Screenshot of last frame before crash
 * 
 * Implementation:
 * - Uses Windows Structured Exception Handling (SEH)
 * - Generates minidumps with DbgHelp.dll
 * - Captures unhandled exceptions
 * 
 * Future Implementations:
 * - Android crash logs (logcat capture)
 * - User-friendly crash dialog with submission
 * - Automatic crash report upload
 * - Crash analytics and grouping
 */
class AnitoCrashReporter {
public:
    using CrashCallback = std::function<void(const std::string& crashDir)>;

    static AnitoCrashReporter* getInstance();
    static void initialize(const std::string& outputDir = "anito-debug/crashes");
    static void destroy();

    /**
     * Set a callback to be called before crash dump is written
     * Useful for capturing last frame, game state, etc.
     */
    void setCrashCallback(CrashCallback callback) { m_crashCallback = callback; }

    /**
     * Get the crash output directory
     */
    std::string getOutputDirectory() const { return m_outputDir; }

    /**
     * Write system information to crash directory
     */
    void writeSystemInfo(const std::string& crashDir);

    /**
     * Enable/disable crash reporting
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    AnitoCrashReporter();
    ~AnitoCrashReporter();
    AnitoCrashReporter(const AnitoCrashReporter&) = delete;
    AnitoCrashReporter& operator=(const AnitoCrashReporter&) = delete;

    void init(const std::string& outputDir);
    void release();
    void ensureOutputDirectoryExists(const std::string& path);
    std::string createCrashDirectory();
    void writeCrashLog(const std::string& crashDir, EXCEPTION_POINTERS* exceptionInfo);
    void writeMinidump(const std::string& crashDir, EXCEPTION_POINTERS* exceptionInfo);

    // Exception handlers
    static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo);
    static void purecallHandler();
    static void invalidParameterHandler(const wchar_t* expression, const wchar_t* function,
                                         const wchar_t* file, unsigned int line, uintptr_t reserved);

    static AnitoCrashReporter* s_instance;

    std::string m_outputDir;
    CrashCallback m_crashCallback;
    bool m_enabled;

    // Previous exception handlers
    LPTOP_LEVEL_EXCEPTION_FILTER m_previousExceptionFilter;
};

} // namespace Anito
