// Copyright 2025 RestingImmortal

#pragma once

#include <entt/entt.hpp>
#include <raylib-cpp.hpp>

#include "AssetManager.hpp"
#include "Events.hpp"
#include "Systems.hpp"

class Game {
public:
    Game(const int width, const int height, const std::string& title) :
        m_window(width, height, title),
        m_camera(
            {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
            {0, 0},
            0.0f,
            1.0f
        ) {
            m_window.SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        }

    void run();

private:
    entt::registry m_registry;
    entt::dispatcher m_dispatcher;
    raylib::Window m_window;
    raylib::Camera2D m_camera;
    AssetManager m_asset_manager;

    void init();

    void update(float dt);

    void render();

    void setup_event_handlers();

    void handle_collision(const Events::Collision& event) {
        on_collision(m_registry, m_asset_manager, event);

    }
};
