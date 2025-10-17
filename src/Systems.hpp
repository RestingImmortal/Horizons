// Copyright 2025 RestingImmortal

#pragma once

#include <entt/entt.hpp>

#include "AssetManager.hpp"
#include "Components.hpp"
#include "Events.hpp"

void camera_to_player(
    entt::registry& registry,
    raylib::Camera2D& camera
);

void despawn_entities(
    entt::registry& registry
);

void engine_visibility(
    entt::registry& registry
);

void load_map(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key
);

void load_start(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key
);

void mark_bullets_for_despawn(
    entt::registry& registry
);

void on_collision(
    entt::registry& registry,
    const Events::Collision& event
);

void player_movement(
    entt::registry& registry,
    AssetManager& asset_manager,
    float dt
);

void print_engine(
    const AssetManager& asset_manager,
    int indentation_level,
    const std::string& engine
);

void print_ship(
    const AssetManager& asset_manager,
    int indentation_level,
    const std::string& ship
);

void print_ship_recurse(
    const AssetManager& asset_manager,
    int indentation_level,
    const std::string& ship
);

void print_weapon(
    const AssetManager& asset_manager,
    int indentation_level,
    const std::string& weapon
);

void render_sprites(
    entt::registry& registry
);

entt::entity spawn_background(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    int layer
);

entt::entity spawn_bullet(
    entt::registry& registry,
    AssetManager& asset_manager,
    Components::Transform& transform,
    const Components::Physics& physics,
    const Components::Weapon& weapon,
    uint32_t affiliation
);

entt::entity spawn_engine(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 relative_offset,
    entt::entity parent_ship
);

entt::entity spawn_object(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position,
    int layer
);

entt::entity spawn_player_ship(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position
);

entt::entity spawn_player_weapon(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 relative_offset,
    entt::entity parent_ship
);

entt::entity spawn_ship(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position,
    uint32_t affiliation
);

entt::entity spawn_weapon(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 relative_offset,
    entt::entity parent_ship
);

void update_background_position(
    entt::registry& registry
);

void update_bullet_timers(
    entt::registry& registry,
    float dt
);

void update_collision(
    entt::registry& registry,
    entt::dispatcher& dispatcher
);

void update_local_transforms(
    entt::registry& registry
);

void update_physics_transforms(
    entt::registry& registry,
    float dt
);

void update_weapon_timers(
    entt::registry& registry,
    float dt
);
