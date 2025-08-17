// Copyright 2025 RestingImmortal

#include <raylib-cpp.hpp>

#include "ConfigManager.hpp"
#include "Game.hpp"

int main() {
    SetTraceLogLevel(LOG_WARNING);

    const ConfigManager configs;

    Game game(800, 600, configs.title);
    game.run();
    return 0;
}
