#pragma once
// Console Logger for Debug Purposes
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

// Include Windows.h first to avoid macro conflicts
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
// Undefine ERROR macro that conflicts with our enum
#ifdef ERROR
#undef ERROR
#endif
#endif

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace Anito {

/**
 * AnitoConsoleLogger - Dual console/file logging system
 * 
 * Features:
 * - Dual output: Console + File simultaneously
 * - Timestamped log entries with precision timing
 * - Log levels: INFO, WARNING, ERROR, DEBUG
 * - Automatic log rotation by session
 * - Thread-safe logging for multi-threaded systems
 * - Color-coded console output (ANSI codes)
 * 
 * Future Implementations:
 * - Log rotation by file size
 * - Log filtering by category/system
 * - Remote logging over network
 * - Log compression for archived logs
 */
class AnitoConsoleLogger {
public:
    enum class LogLevel {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };

    static AnitoConsoleLogger* getInstance();
    static void initialize(const std::string& outputDir = "anito-debug/logs");
    static void destroy();

    /**
     * Log a message with specified level and category
     * @param level Log severity level
     * @param category System/category name (e.g., "Engine", "Renderer", "Physics")
     * @param message Log message
     */
    void log(LogLevel level, const std::string& category, const std::string& message);

    /**
     * Convenience methods for different log levels
     */
    void logDebug(const std::string& category, const std::string& message);
    void logInfo(const std::string& category, const std::string& message);
    void logWarning(const std::string& category, const std::string& message);
    void logError(const std::string& category, const std::string& message);

    /**
     * Set minimum log level to output (filters lower priority logs)
     * Default: INFO
     */
    void setMinLogLevel(LogLevel level) { m_minLogLevel = level; }

    /**
     * Enable/disable file logging
     */
    void setFileLoggingEnabled(bool enabled) { m_fileLoggingEnabled = enabled; }

    /**
     * Enable/disable console logging
     */
    void setConsoleLoggingEnabled(bool enabled) { m_consoleLoggingEnabled = enabled; }

    /**
     * Enable/disable color output in console
     */
    void setColorOutputEnabled(bool enabled) { m_colorOutputEnabled = enabled; }

    /**
     * Flush all pending writes to file
     */
    void flush();

    /**
     * Get the current log file path
     */
    std::string getCurrentLogFilePath() const { return m_currentLogFilePath; }

private:
    AnitoConsoleLogger();
    ~AnitoConsoleLogger();
    AnitoConsoleLogger(const AnitoConsoleLogger&) = delete;
    AnitoConsoleLogger& operator=(const AnitoConsoleLogger&) = delete;

    void init(const std::string& outputDir);
    void release();

    void ensureOutputDirectoryExists(const std::string& path);
    std::string generateLogFileName();
    std::string getCurrentTimestamp();
    std::string getLevelString(LogLevel level);
    std::string getLevelColorCode(LogLevel level);
    void writeToFile(const std::string& formattedMessage);
    void writeToConsole(const std::string& formattedMessage, LogLevel level);

    static AnitoConsoleLogger* s_instance;

    std::string m_outputDir;
    std::string m_currentLogFilePath;
    std::ofstream m_logFile;
    std::mutex m_logMutex;

    LogLevel m_minLogLevel;
    bool m_fileLoggingEnabled;
    bool m_consoleLoggingEnabled;
    bool m_colorOutputEnabled;
};

} // namespace Anito

// Convenient macros for logging
#define ANITO_LOG_DEBUG(category, message) \
    if (Anito::AnitoConsoleLogger::getInstance()) \
        Anito::AnitoConsoleLogger::getInstance()->logDebug(category, message)

#define ANITO_LOG_INFO(category, message) \
    if (Anito::AnitoConsoleLogger::getInstance()) \
        Anito::AnitoConsoleLogger::getInstance()->logInfo(category, message)

#define ANITO_LOG_WARNING(category, message) \
    if (Anito::AnitoConsoleLogger::getInstance()) \
        Anito::AnitoConsoleLogger::getInstance()->logWarning(category, message)

#define ANITO_LOG_ERROR(category, message) \
    if (Anito::AnitoConsoleLogger::getInstance()) \
        Anito::AnitoConsoleLogger::getInstance()->logError(category, message)
