// Copyright 2025 RestingImmortal

#pragma once

#include <print>

#include <entt/entt.hpp>
#include <raylib-cpp.hpp>

#include "AssetManager.hpp"

class Game {
public:
    Game(int width, int height, const char* title)
        : m_window(width, height, title),
          m_camera({GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f}, {0, 0}, 0.0f, 1.0f) {
            m_window.SetConfigFlags(FLAG_WINDOW_RESIZABLE);
          }
    
    void run();

private:
    entt::registry m_registry;
    raylib::Window m_window;
    raylib::Camera2D m_camera;
    AssetManager m_asset_manager;

    void init();

    void update(float dt);

    void render();
};
