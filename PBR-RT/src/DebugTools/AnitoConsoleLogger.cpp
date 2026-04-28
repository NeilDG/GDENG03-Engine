#include "AnitoConsoleLogger.h"
#include <iostream>
#include <filesystem>
#include <chrono>

namespace Anito {

AnitoConsoleLogger* AnitoConsoleLogger::s_instance = nullptr;

AnitoConsoleLogger* AnitoConsoleLogger::getInstance() {
    return s_instance;
}

void AnitoConsoleLogger::initialize(const std::string& outputDir) {
    if (s_instance) {
        std::cerr << "[ConsoleLogger] Already initialized!" << std::endl;
        return;
    }

    s_instance = new AnitoConsoleLogger();
    s_instance->init(outputDir);
}

void AnitoConsoleLogger::destroy() {
    if (s_instance) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoConsoleLogger::AnitoConsoleLogger()
    : m_outputDir("anito-debug/logs")
    , m_minLogLevel(LogLevel::INFO)
    , m_fileLoggingEnabled(true)
    , m_consoleLoggingEnabled(true)
    , m_colorOutputEnabled(true)
{
}

AnitoConsoleLogger::~AnitoConsoleLogger() {
    release();
}

void AnitoConsoleLogger::init(const std::string& outputDir) {
    m_outputDir = outputDir;
    ensureOutputDirectoryExists(m_outputDir);

    // Generate log file name with timestamp
    m_currentLogFilePath = m_outputDir + "/" + generateLogFileName();

    // Open log file
    m_logFile.open(m_currentLogFilePath, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "[ConsoleLogger] Failed to open log file: " << m_currentLogFilePath << std::endl;
        m_fileLoggingEnabled = false;
        return;
    }

    // Enable ANSI color codes on Windows (Windows 10+)
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(hConsole, &mode)) {
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, mode);
    }
#endif

    // Write session header
    std::string sessionHeader = "\n";
    sessionHeader += "================================================================================\n";
    sessionHeader += "  Anito Engine - New Session Started\n";
    sessionHeader += "  Timestamp: " + getCurrentTimestamp() + "\n";
    sessionHeader += "  Log File: " + m_currentLogFilePath + "\n";
    sessionHeader += "================================================================================\n";

    if (m_fileLoggingEnabled) {
        m_logFile << sessionHeader << std::endl;
        m_logFile.flush();
    }

    if (m_consoleLoggingEnabled) {
        std::cout << sessionHeader << std::endl;
    }
}

void AnitoConsoleLogger::release() {
    if (m_logFile.is_open()) {
        // Write session footer
        std::string sessionFooter = "\n";
        sessionFooter += "================================================================================\n";
        sessionFooter += "  Session Ended\n";
        sessionFooter += "  Timestamp: " + getCurrentTimestamp() + "\n";
        sessionFooter += "================================================================================\n";

        m_logFile << sessionFooter << std::endl;
        m_logFile.close();
    }
}

void AnitoConsoleLogger::log(LogLevel level, const std::string& category, const std::string& message) {
    // Filter by log level
    if (level < m_minLogLevel) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_logMutex);

    // Format: [2025-01-20 14:35:22.142] [INFO] [Engine] Anito Engine initialized
    std::ostringstream oss;
    oss << "[" << getCurrentTimestamp() << "] "
        << "[" << getLevelString(level) << "] "
        << "[" << category << "] "
        << message;

    std::string formattedMessage = oss.str();

    // Write to file
    if (m_fileLoggingEnabled) {
        writeToFile(formattedMessage);
    }

    // Write to console
    if (m_consoleLoggingEnabled) {
        writeToConsole(formattedMessage, level);
    }
}

void AnitoConsoleLogger::logDebug(const std::string& category, const std::string& message) {
    log(LogLevel::DEBUG, category, message);
}

void AnitoConsoleLogger::logInfo(const std::string& category, const std::string& message) {
    log(LogLevel::INFO, category, message);
}

void AnitoConsoleLogger::logWarning(const std::string& category, const std::string& message) {
    log(LogLevel::WARNING, category, message);
}

void AnitoConsoleLogger::logError(const std::string& category, const std::string& message) {
    log(LogLevel::ERROR, category, message);
}

void AnitoConsoleLogger::flush() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    if (m_logFile.is_open()) {
        m_logFile.flush();
    }
}

void AnitoConsoleLogger::ensureOutputDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[ConsoleLogger] Failed to create output directory: " << e.what() << std::endl;
    }
}

std::string AnitoConsoleLogger::generateLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    struct tm timeinfo;
    localtime_s(&timeinfo, &time);

    std::ostringstream oss;
    oss << "anito_"
        << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S")
        << ".log";

    return oss.str();
}

std::string AnitoConsoleLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    struct tm timeinfo;
    localtime_s(&timeinfo, &time);

    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

std::string AnitoConsoleLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG  ";
        case LogLevel::INFO:    return "INFO   ";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR  ";
        default:                return "UNKNOWN";
    }
}

std::string AnitoConsoleLogger::getLevelColorCode(LogLevel level) {
    if (!m_colorOutputEnabled) {
        return "";
    }

    // ANSI color codes
    switch (level) {
        case LogLevel::DEBUG:   return "\033[36m"; // Cyan
        case LogLevel::INFO:    return "\033[37m"; // White
        case LogLevel::WARNING: return "\033[33m"; // Yellow
        case LogLevel::ERROR:   return "\033[31m"; // Red
        default:                return "\033[37m"; // White
    }
}

void AnitoConsoleLogger::writeToFile(const std::string& formattedMessage) {
    if (m_logFile.is_open()) {
        m_logFile << formattedMessage << std::endl;
    }
}

void AnitoConsoleLogger::writeToConsole(const std::string& formattedMessage, LogLevel level) {
    std::string colorCode = getLevelColorCode(level);
    std::string resetCode = m_colorOutputEnabled ? "\033[0m" : "";

    // Output to appropriate stream
    if (level >= LogLevel::WARNING) {
        std::cerr << colorCode << formattedMessage << resetCode << std::endl;
    }
    else {
        std::cout << colorCode << formattedMessage << resetCode << std::endl;
    }
}

} // namespace Anito
