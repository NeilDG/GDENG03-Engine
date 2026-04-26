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

        engine.run();
        // Note: shutdown() is called automatically in ~AnitoEngine()

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}
