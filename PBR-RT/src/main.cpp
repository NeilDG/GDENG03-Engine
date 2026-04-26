// main.cpp - Anito Engine Entry Point
// Project Anito - DOST-PCIEERD Funded Initiative
// DLSU GAME Lab

#include "AnitoEngine.h"
#include <iostream>
#include <exception>

int main(int argc, char** argv) {
    try {
        Anito::AnitoEngine engine;

        if (!engine.initialize("Anito Engine - Deferred PBR Renderer", 1280, 720)) {
            std::cerr << "Failed to initialize Anito Engine!" << std::endl;
            return -1;
        }

        // Enable auto-shutdown after 6 seconds for frame capture testing
        engine.setAutoShutdown(6.0f);
        std::cout << "[Main] Auto-shutdown enabled: Engine will close after 6 seconds" << std::endl;

        engine.run();
        engine.shutdown();

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}
