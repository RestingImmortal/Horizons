// Copyright 2025 RestingImmortal

#include "ConfigManager.hpp"

#include <fstream>
#include <print>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

ConfigManager::ConfigManager() {
    try {
        std::ifstream file("./META.json");
        json jsonData = json::parse(file);
        title = jsonData.value("title", "Untitled Game");
        log_level = Logger::from_string(jsonData.value("log_level", "Warning"));
    } catch (const std::exception& e) {
        std::println("Error initializing game: {}", e.what());
        throw std::runtime_error("Couldn't initialize game.");
    }
}
