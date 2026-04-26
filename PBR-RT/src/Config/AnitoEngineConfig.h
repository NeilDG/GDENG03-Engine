#pragma once
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Anito {

/**
 * AnitoEngineConfig - Simple INI-style configuration parser
 * Allows easy modification of engine runtime parameters by users and AI agents
 */
class AnitoEngineConfig {
public:
    /**
     * Load configuration from file
     * @param filepath Path to configuration file (default: "engine_config.ini")
     * @return true if file loaded successfully, false otherwise
     */
    static bool load(const std::string& filepath = "engine_config.ini") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cout << "[Config] Configuration file not found: " << filepath << std::endl;
            std::cout << "[Config] Using default settings." << std::endl;
            return false;
        }

        std::string line;
        std::string currentSection;

        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            // Check for section header
            if (line[0] == '[' && line[line.length() - 1] == ']') {
                currentSection = line.substr(1, line.length() - 2);
                continue;
            }

            // Parse key-value pair
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);

                // Trim key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                // Store with section prefix
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                s_config[fullKey] = value;
            }
        }

        file.close();
        std::cout << "[Config] Configuration loaded from: " << filepath << std::endl;
        return true;
    }

    /**
     * Get string value from configuration
     */
    static std::string getString(const std::string& key, const std::string& defaultValue = "") {
        auto it = s_config.find(key);
        if (it != s_config.end()) {
            return it->second;
        }
        return defaultValue;
    }

    /**
     * Get integer value from configuration
     */
    static int getInt(const std::string& key, int defaultValue = 0) {
        auto it = s_config.find(key);
        if (it != s_config.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                std::cerr << "[Config] Failed to parse integer for key: " << key << std::endl;
            }
        }
        return defaultValue;
    }

    /**
     * Get float value from configuration
     */
    static float getFloat(const std::string& key, float defaultValue = 0.0f) {
        auto it = s_config.find(key);
        if (it != s_config.end()) {
            try {
                return std::stof(it->second);
            } catch (...) {
                std::cerr << "[Config] Failed to parse float for key: " << key << std::endl;
            }
        }
        return defaultValue;
    }

    /**
     * Get boolean value from configuration
     */
    static bool getBool(const std::string& key, bool defaultValue = false) {
        auto it = s_config.find(key);
        if (it != s_config.end()) {
            std::string value = it->second;
            // Convert to lowercase for comparison
            for (char& c : value) {
                c = static_cast<char>(std::tolower(c));
            }
            return value == "true" || value == "1" || value == "yes" || value == "on";
        }
        return defaultValue;
    }

    /**
     * Print all loaded configuration values (for debugging)
     */
    static void printAll() {
        std::cout << "[Config] Current configuration:" << std::endl;
        for (const auto& pair : s_config) {
            std::cout << "  " << pair.first << " = " << pair.second << std::endl;
        }
    }

private:
    static std::map<std::string, std::string> s_config;
};

// Static member definition
std::map<std::string, std::string> AnitoEngineConfig::s_config;

} // namespace Anito
