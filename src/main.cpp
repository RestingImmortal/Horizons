// Copyright 2025 RestingImmortal

#include <raylib-cpp.hpp>

#include "ConfigManager.hpp"
#include "Game.hpp"
#include "Logger.hpp"

int main() {
    // Library configuration
    SetTraceLogLevel(LOG_WARNING);

    // Horizons configuration
    const ConfigManager configs;

    Logger::set_level(configs.log_level);
    Logger::get().add_sink(std::make_unique<ConsoleSink>());

    // Game
    Game game(800, 600, configs.title);
    game.run();

    // Exiting
    return 0;
}
