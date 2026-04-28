#include "AnitoFrameCaptureRecorder.h"
#include "../Renderer/AnitoRenderer.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

// STB Image for reading TGA files (NO IMPLEMENTATION - already defined in AnitoRenderer.cpp)
#include "../../external/bgfx/bimg/3rdparty/stb/stb_image.h"

// STB Image Write for saving PNG/JPG
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../external/stb/stb_image_write.h"

namespace Anito {

AnitoFrameCaptureRecorder::AnitoFrameCaptureRecorder()
    : m_outputDir("anito-debug/frames")
    , m_format("png")
    , m_jpgQuality(90)
    , m_captureInterval(3.0f)
    , m_timeSinceLastCapture(0.0f)
    , m_frameCount(0)
    , m_autoCapture(true)
    , m_width(0)
    , m_height(0)
{
}

AnitoFrameCaptureRecorder::~AnitoFrameCaptureRecorder() {
}

void AnitoFrameCaptureRecorder::initialize(const std::string& outputDir, 
                                            float captureIntervalSeconds,
                                            const std::string& format,
                                            int jpgQuality) {
    m_outputDir = outputDir;
    m_captureInterval = captureIntervalSeconds;
    m_format = format;
    m_jpgQuality = jpgQuality;
    m_timeSinceLastCapture = 0.0f;
    m_frameCount = 0;

    // Validate format
    if (m_format != "png" && m_format != "jpg") {
        std::cerr << "[FrameCapture] Invalid format '" << m_format << "', defaulting to PNG" << std::endl;
        m_format = "png";
    }

    // Ensure output directory exists
    ensureOutputDirectoryExists();

    std::cout << "[FrameCapture] Initialized:" << std::endl;
    std::cout << "  Output Dir: " << m_outputDir << std::endl;
    std::cout << "  Interval: " << m_captureInterval << "s" << std::endl;
    std::cout << "  Format: " << m_format << std::endl;
    if (m_format == "jpg") {
        std::cout << "  JPG Quality: " << m_jpgQuality << std::endl;
    }
}

void AnitoFrameCaptureRecorder::update(float deltaTime) {
    // Process any pending TGA to PNG/JPG conversions
    processConversions();

    if (!m_autoCapture) return;

    m_timeSinceLastCapture += deltaTime;

    if (m_timeSinceLastCapture >= m_captureInterval) {
        captureFrame();
        m_timeSinceLastCapture = 0.0f;
    }
}

void AnitoFrameCaptureRecorder::captureFrame() {
    AnitoRenderer* renderer = AnitoRenderer::getInstance();
    if (!renderer) {
        std::cerr << "[FrameCapture] Error: Renderer not available" << std::endl;
        return;
    }

    m_width = renderer->getWidth();
    m_height = renderer->getHeight();

    // Generate final PNG/JPG filename with zero-padded frame number
    std::ostringstream finalOss;
    finalOss << m_outputDir << "/frame_" << std::setfill('0') << std::setw(4) << m_frameCount << "." << m_format;
    std::string finalFilename = finalOss.str();

    std::cout << "[FrameCapture] Capturing frame " << m_frameCount << " -> " << finalFilename << std::endl;

    // Create temp filename WITHOUT extension (bgfx will add .tga automatically)
    // Format: anito-debug/frames/temp_frame_0 (bgfx makes it temp_frame_0.tga)
    std::ostringstream tempOss;
    tempOss << m_outputDir << "/temp_frame_" << m_frameCount;
    std::string tempTgaBase = tempOss.str();
    std::string tempTgaFilename = tempTgaBase + ".tga"; // This is what bgfx will actually create

    // Request screenshot - bgfx will automatically append .tga to tempTgaBase
    bgfx::requestScreenShot(BGFX_INVALID_HANDLE, tempTgaBase.c_str());

    // Queue for conversion
    m_pendingConversions.push_back({tempTgaFilename, finalFilename, m_frameCount});

    m_frameCount++;
}

void AnitoFrameCaptureRecorder::ensureOutputDirectoryExists() {
    try {
        if (!std::filesystem::exists(m_outputDir)) {
            std::filesystem::create_directories(m_outputDir);
            std::cout << "[FrameCapture] Created output directory: " << m_outputDir << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[FrameCapture] Failed to create output directory: " << e.what() << std::endl;
    }
}

void AnitoFrameCaptureRecorder::processConversions() {
    // Process pending conversions (TGA files that bgfx has written)
    auto it = m_pendingConversions.begin();
    while (it != m_pendingConversions.end()) {
        // Check if TGA file exists (bgfx may take a frame to write it)
        if (std::filesystem::exists(it->tgaPath)) {
            std::cout << "[FrameCapture] Converting frame " << it->frameNum << " to " << m_format << "..." << std::endl;

            if (convertTgaToPngOrJpg(it->tgaPath, it->targetPath)) {
                // Delete temporary TGA file after successful conversion
                try {
                    std::filesystem::remove(it->tgaPath);
                    std::cout << "[FrameCapture] Frame " << it->frameNum << " saved: " << it->targetPath << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "[FrameCapture] Failed to delete temp TGA: " << e.what() << std::endl;
                }
            }
            else {
                std::cerr << "[FrameCapture] Failed to convert frame " << it->frameNum << std::endl;
            }

            // Remove from pending list
            it = m_pendingConversions.erase(it);
        }
        else {
            // TGA not ready yet, try again next frame
            ++it;
        }
    }
}

bool AnitoFrameCaptureRecorder::convertTgaToPngOrJpg(const std::string& tgaPath, const std::string& targetPath) {
    // Read TGA file using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(tgaPath.c_str(), &width, &height, &channels, 4); // Force RGBA

    if (!data) {
        std::cerr << "[FrameCapture] Failed to load TGA: " << tgaPath << std::endl;
        std::cerr << "[FrameCapture] STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // Flip image vertically (OpenGL origin is bottom-left, image files expect top-left)
    int rowSize = width * 4; // 4 bytes per pixel (RGBA)
    std::vector<unsigned char> tempRow(rowSize);

    for (int y = 0; y < height / 2; ++y) {
        unsigned char* row1 = data + y * rowSize;
        unsigned char* row2 = data + (height - 1 - y) * rowSize;

        // Swap rows
        std::memcpy(tempRow.data(), row1, rowSize);
        std::memcpy(row1, row2, rowSize);
        std::memcpy(row2, tempRow.data(), rowSize);
    }

    bool success = false;

    // Write as PNG or JPG
    if (m_format == "png") {
        // PNG supports alpha channel
        success = stbi_write_png(targetPath.c_str(), width, height, 4, data, width * 4) != 0;
    }
    else if (m_format == "jpg") {
        // JPG doesn't support alpha, convert RGBA to RGB
        std::vector<unsigned char> rgbData(width * height * 3);
        for (int i = 0; i < width * height; ++i) {
            rgbData[i * 3 + 0] = data[i * 4 + 0]; // R
            rgbData[i * 3 + 1] = data[i * 4 + 1]; // G
            rgbData[i * 3 + 2] = data[i * 4 + 2]; // B
        }
        success = stbi_write_jpg(targetPath.c_str(), width, height, 3, rgbData.data(), m_jpgQuality) != 0;
    }

    // Free loaded image data
    stbi_image_free(data);

    return success;
}

} // namespace Anito
