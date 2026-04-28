#include "AnitoCrashReporter.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

namespace Anito {

AnitoCrashReporter* AnitoCrashReporter::s_instance = nullptr;

AnitoCrashReporter* AnitoCrashReporter::getInstance() {
    return s_instance;
}

void AnitoCrashReporter::initialize(const std::string& outputDir) {
    if (s_instance) {
        std::cerr << "[CrashReporter] Already initialized!" << std::endl;
        return;
    }

    s_instance = new AnitoCrashReporter();
    s_instance->init(outputDir);
}

void AnitoCrashReporter::destroy() {
    if (s_instance) {
        s_instance->release();
        delete s_instance;
        s_instance = nullptr;
    }
}

AnitoCrashReporter::AnitoCrashReporter()
    : m_outputDir("anito-debug/crashes")
    , m_enabled(true)
    , m_previousExceptionFilter(nullptr)
{
}

AnitoCrashReporter::~AnitoCrashReporter() {
    release();
}

void AnitoCrashReporter::init(const std::string& outputDir) {
    m_outputDir = outputDir;
    ensureOutputDirectoryExists(m_outputDir);

    // Set up exception handlers
    m_previousExceptionFilter = SetUnhandledExceptionFilter(unhandledExceptionFilter);
    _set_purecall_handler(purecallHandler);
    _set_invalid_parameter_handler(invalidParameterHandler);

    std::cout << "[CrashReporter] Initialized" << std::endl;
    std::cout << "  Output Dir: " << m_outputDir << std::endl;
    std::cout << "  Minidump enabled (Windows)" << std::endl;
}

void AnitoCrashReporter::release() {
    // Restore previous exception handlers
    if (m_previousExceptionFilter) {
        SetUnhandledExceptionFilter(m_previousExceptionFilter);
    }
}

LONG WINAPI AnitoCrashReporter::unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
    if (!s_instance || !s_instance->m_enabled) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    std::cerr << "\n========================================" << std::endl;
    std::cerr << "UNHANDLED EXCEPTION DETECTED!" << std::endl;
    std::cerr << "========================================" << std::endl;

    // Create crash directory
    std::string crashDir = s_instance->createCrashDirectory();

    // Call user callback (e.g., to capture last frame)
    if (s_instance->m_crashCallback) {
        try {
            s_instance->m_crashCallback(crashDir);
        }
        catch (...) {
            std::cerr << "[CrashReporter] Exception in crash callback!" << std::endl;
        }
    }

    // Write crash log
    s_instance->writeCrashLog(crashDir, exceptionInfo);

    // Write minidump
    s_instance->writeMinidump(crashDir, exceptionInfo);

    // Write system info
    s_instance->writeSystemInfo(crashDir);

    std::cerr << "[CrashReporter] Crash data saved to: " << crashDir << std::endl;
    std::cerr << "========================================" << std::endl;

    // Show message box
    std::string message = "Anito Engine has crashed!\n\nCrash data saved to:\n" + crashDir;
    MessageBoxA(NULL, message.c_str(), "Anito Engine - Crash Detected", MB_OK | MB_ICONERROR);

    return EXCEPTION_EXECUTE_HANDLER;
}

void AnitoCrashReporter::purecallHandler() {
    std::cerr << "[CrashReporter] Pure virtual function call detected!" << std::endl;
    RaiseException(0xE06D7363, 0, 0, nullptr); // C++ exception code
}

void AnitoCrashReporter::invalidParameterHandler(const wchar_t* expression, const wchar_t* function,
                                                  const wchar_t* file, unsigned int line, uintptr_t reserved) {
    std::wcerr << L"[CrashReporter] Invalid parameter detected!" << std::endl;
    std::wcerr << L"  Expression: " << (expression ? expression : L"(null)") << std::endl;
    std::wcerr << L"  Function: " << (function ? function : L"(null)") << std::endl;
    std::wcerr << L"  File: " << (file ? file : L"(null)") << std::endl;
    std::wcerr << L"  Line: " << line << std::endl;

    RaiseException(0xE06D7363, 0, 0, nullptr);
}

std::string AnitoCrashReporter::createCrashDirectory() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);

    std::ostringstream oss;
    oss << m_outputDir << "/crash_"
        << std::put_time(&timeinfo, "%Y-%m-%d_%H-%M-%S");

    std::string crashDir = oss.str();
    ensureOutputDirectoryExists(crashDir);

    return crashDir;
}

void AnitoCrashReporter::writeCrashLog(const std::string& crashDir, EXCEPTION_POINTERS* exceptionInfo) {
    std::string logPath = crashDir + "/crash_log.txt";
    std::ofstream file(logPath);

    if (!file.is_open()) {
        std::cerr << "[CrashReporter] Failed to create crash log: " << logPath << std::endl;
        return;
    }

    file << "========================================" << std::endl;
    file << "Anito Engine - Crash Report" << std::endl;
    file << "========================================" << std::endl;

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    file << "Timestamp: " << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << std::endl;

    if (exceptionInfo && exceptionInfo->ExceptionRecord) {
        PEXCEPTION_RECORD record = exceptionInfo->ExceptionRecord;
        file << "\nException Information:" << std::endl;
        file << "  Code: 0x" << std::hex << record->ExceptionCode << std::dec << std::endl;
        file << "  Address: 0x" << std::hex << record->ExceptionAddress << std::dec << std::endl;
        file << "  Flags: 0x" << std::hex << record->ExceptionFlags << std::dec << std::endl;

        // Exception code descriptions
        switch (record->ExceptionCode) {
            case EXCEPTION_ACCESS_VIOLATION:
                file << "  Description: Access Violation" << std::endl;
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                file << "  Description: Array Bounds Exceeded" << std::endl;
                break;
            case EXCEPTION_BREAKPOINT:
                file << "  Description: Breakpoint" << std::endl;
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                file << "  Description: Datatype Misalignment" << std::endl;
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                file << "  Description: Float Divide by Zero" << std::endl;
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                file << "  Description: Integer Divide by Zero" << std::endl;
                break;
            case EXCEPTION_STACK_OVERFLOW:
                file << "  Description: Stack Overflow" << std::endl;
                break;
            default:
                file << "  Description: Unknown Exception" << std::endl;
                break;
        }
    }

    file << "\nFor detailed analysis, load the minidump in Visual Studio:" << std::endl;
    file << "  File -> Open -> File -> crash.dmp" << std::endl;
    file << "  Click 'Debug with Native Only' to analyze the crash" << std::endl;

    file.close();
    std::cout << "[CrashReporter] Crash log written: " << logPath << std::endl;
}

void AnitoCrashReporter::writeMinidump(const std::string& crashDir, EXCEPTION_POINTERS* exceptionInfo) {
    std::string dumpPath = crashDir + "/crash.dmp";

    HANDLE hFile = CreateFileA(dumpPath.c_str(), GENERIC_WRITE, 0, nullptr,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "[CrashReporter] Failed to create minidump file: " << dumpPath << std::endl;
        return;
    }

    MINIDUMP_EXCEPTION_INFORMATION mdei;
    mdei.ThreadId = GetCurrentThreadId();
    mdei.ExceptionPointers = exceptionInfo;
    mdei.ClientPointers = FALSE;

    MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
        MiniDumpWithDataSegs |
        MiniDumpWithHandleData |
        MiniDumpWithUnloadedModules |
        MiniDumpWithProcessThreadData |
        MiniDumpWithFullMemoryInfo |
        MiniDumpWithThreadInfo
    );

    BOOL success = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        exceptionInfo ? &mdei : nullptr,
        nullptr,
        nullptr
    );

    CloseHandle(hFile);

    if (success) {
        std::cout << "[CrashReporter] Minidump written: " << dumpPath << std::endl;
    }
    else {
        std::cerr << "[CrashReporter] Failed to write minidump!" << std::endl;
    }
}

void AnitoCrashReporter::writeSystemInfo(const std::string& crashDir) {
    std::string infoPath = crashDir + "/system_info.txt";
    std::ofstream file(infoPath);

    if (!file.is_open()) {
        std::cerr << "[CrashReporter] Failed to create system info: " << infoPath << std::endl;
        return;
    }

    file << "========================================" << std::endl;
    file << "System Information" << std::endl;
    file << "========================================" << std::endl;

    // OS Version
    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

    // Note: GetVersionEx is deprecated, but still works for crash reporting
    #pragma warning(push)
    #pragma warning(disable: 4996)
    GetVersionExA((OSVERSIONINFOA*)&osvi);
    #pragma warning(pop)

    file << "OS Version: " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion 
         << " Build " << osvi.dwBuildNumber << std::endl;

    // Memory
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    file << "Total Physical Memory: " << (memInfo.ullTotalPhys / (1024 * 1024)) << " MB" << std::endl;
    file << "Available Physical Memory: " << (memInfo.ullAvailPhys / (1024 * 1024)) << " MB" << std::endl;
    file << "Memory Usage: " << memInfo.dwMemoryLoad << "%" << std::endl;

    // CPU
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    file << "CPU Cores: " << sysInfo.dwNumberOfProcessors << std::endl;

    file.close();
    std::cout << "[CrashReporter] System info written: " << infoPath << std::endl;
}

void AnitoCrashReporter::ensureOutputDirectoryExists(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[CrashReporter] Failed to create output directory: " << e.what() << std::endl;
    }
}

} // namespace Anito
