// Copyright 2025 RestingImmortal

#pragma once

#include <print>

#include <entt/entt.hpp>
#include <raylib-cpp.hpp>

#include "AssetManager.hpp"
#include "Timer.hpp"

namespace Components {

    struct Affiliation {
        uint32_t id;
    };

    struct Animation {
        std::vector<std::string> frames;
        float frame_duration = 1.0f;
        Timer timer;
        size_t current_frame;
    };

    struct Background {};

    struct Bullet {
        float damage = 0.0;
        float lifetime = 0.0;
        Timer despawn_timer;
    };

    struct Collider {
        float radius = 0.0f;
        uint32_t category = 0;
        uint32_t collides_with = 0;
    };

    struct DespawnMarker {};

    struct Engine {
        float thrust = 20.0f;
    };

    struct HullHealth {
        float hull_front = 1.0f;
        float hull_right = 1.0f;
        float hull_back = 1.0f;
        float hull_left = 1.0f;
    };

    struct Parent {
        entt::entity parent;
    };

    struct Physics {
        float acceleration = 20.0f;
        float max_speed = 400.0f;
        raylib::Vector2 velocity = {0.0f, 0.0f};
        float rotation = 180.0f;
    };

    struct Player {};

    struct PlayerWeapon {};

    struct RelativeTransform {
        raylib::Vector2 offset = {0.0f, 0.0f};
        float size = 1.0f;
        float rotation = 0.0f;
    };

    struct Renderable {
        raylib::Color color = raylib::Color::White();
        raylib::TextureUnmanaged texture;
    };

    struct RenderOrder {
        int layer = 0;
    };

    struct ShouldNotRender {};

    struct Thrusting {
        bool active = false;
    };

    struct Transform {
        raylib::Vector2 position = {0.0f, 0.0f};
        raylib::Vector2 size = {1, 1};
        float rotation = 0.0f;
    };

    struct Weapon {
        Weapon() = default;
        Weapon(const std::string& key, const AssetManager& asset_manager);

        std::string munition;
        float damage = 0.0f;
        float lifetime = 0.01f;
        float cooldown = 2'000'000.0f;
        float radius = 0.0f;
        float shot_speed = 100;
        Timer fire_timer;

        bool can_fire() const;

        void trigger_cooldown();
    };
}
