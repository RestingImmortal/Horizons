// Copyright 2025 RestingImmortal

#include "Game.hpp"

#include "Components.hpp"
#include "Systems.hpp"
#include "raylib.h"

// Public Methods

void Game::run() {
    init();

    while (!m_window.ShouldClose()) {
        const float dt = m_window.GetFrameTime();
        update(dt);
        render();
    }
}

// Private Methods

void Game::init() {
    SetTargetFPS(60);

    m_asset_manager.load_assets();

    load_start(
        m_registry,
        m_asset_manager,
        "Multi-start unsupported still. Harass Luna if this is limiting you"
    );
}

void Game::update(const float dt) {
    update_weapon_timers(m_registry, dt);
    update_bullet_timers(m_registry, dt);
    player_movement(m_registry, m_asset_manager, dt);
    update_physics_transforms(m_registry, dt);
    update_local_transforms(m_registry);
    update_background_position(m_registry);
    engine_visibility(m_registry);
    mark_bullets_for_despawn(m_registry);
    camera_to_player(m_registry, m_camera);
    despawn_entities(m_registry);
}

void Game::render() {
    m_window.BeginDrawing();
        m_window.ClearBackground(raylib::Color::Black());

        m_camera.BeginMode();
            render_sprites(m_registry);
        m_camera.EndMode();

    m_window.EndDrawing();
}
