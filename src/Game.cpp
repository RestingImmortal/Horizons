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

    // Debug printing
    const auto ship = m_asset_manager.get_ship("example");
    std::println("~~~~~~~\nExample ship:");
    if (!ship) { std::println("Ruh roh!"); }
    else {
        const auto ship_data = *ship;
        std::println("Texture: {}", ship_data->texture);
        for (auto weapon : ship_data->weapons) {
            std::println("Weapon type: {}", weapon.weapon_type);
            if (auto weapon_result = m_asset_manager.get_weapon(weapon.weapon_type);
                !weapon_result) { std::println("Ruh roh!"); }
            else {
                std::println("Weapon munition: {}", (*weapon_result)->munition);
                std::println("Weapon damage: {}", (*weapon_result)->damage);
                std::println("Weapon lifetime: {}", (*weapon_result)->lifetime);
                std::println("Weapon cooldown: {}", (*weapon_result)->cooldown);
            }
            std::println("Weapon coords: {}, {}", weapon.x, weapon.y);
        }
        for (auto engine : ship_data->engines) {
            std::println("Engine type: {}", engine.engine_type);
            if (auto engine_result = m_asset_manager.get_engine(engine.engine_type);
                !engine_result) { std::println("Ruh roh!"); }
            else {
                std::println("Engine texture: {}", (*engine_result)->texture);
                std::println("Engine thrust: {}", (*engine_result)->thrust);
            }
            std::println("Engine coords: {}, {}", engine.x, engine.y);
        }
    }
    std::println("~~~~~~~");

    // PLAYER
    spawn_player_ship(
        m_registry,
        m_asset_manager,
        "example",
        raylib::Vector2{200.0, 200.0}
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
