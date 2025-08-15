// Copyright 2025 RestingImmortal

#include <raylib-cpp.hpp>

#include "Game.hpp"

int main() {
    SetTraceLogLevel(LOG_WARNING);

    Game game(800, 600, "Atherium: Dominion INDEV");
    game.run();
    return 0;
}
