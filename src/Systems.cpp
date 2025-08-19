// Copyright 2025 RestingImmortal

#include "Systems.hpp"

#include <cmath>
#include <print>

#include <raylib-cpp.hpp>

#include "AssetManager.hpp"
#include "Components.hpp"
#include "Timer.hpp"

void camera_to_player(
    entt::registry& registry,
    raylib::Camera2D& camera
) {
    const auto view = registry.view<Components::Transform, Components::Player>();
    view.each([&camera](const auto& transform) {
        camera.SetOffset(raylib::Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});
        camera.SetTarget(transform.position);
    });
}

void despawn_entities(entt::registry &registry) {
    const auto view = registry.view<Components::DespawnMarker>();
    std::vector<entt::entity> entities_to_destroy;

    for (auto entity : view) {
        entities_to_destroy.push_back(entity);
    }

    registry.destroy(entities_to_destroy.begin(), entities_to_destroy.end());
}

void engine_visibility(entt::registry &registry) {
    for (const auto entity : registry.view<Components::Engine, Components::Parent>()) {
        const auto parent = registry.get<Components::Parent>(entity).parent;
        const bool thrusting = registry.any_of<Components::Thrusting>(parent)
                       && registry.get<Components::Thrusting>(parent).active;
        
        if (!thrusting) {
            registry.emplace_or_replace<Components::ShouldNotRender>(entity);
        } else if (registry.any_of<Components::ShouldNotRender>(entity)) {
            registry.remove<Components::ShouldNotRender>(entity);
        }
    }
}

void mark_bullets_for_despawn(entt::registry &registry) {
    for (const auto view = registry.view<Components::Bullet>();
        const auto entity : view) {
        if (auto& bullet = registry.get<Components::Bullet>(entity);
            bullet.despawn_timer.is_done()) {
            registry.emplace<Components::DespawnMarker>(entity);
        }
    }
}

void player_movement(
    entt::registry& registry,
    AssetManager& asset_manager,
    const float dt
) {
    for (
        const auto view = registry.view<
            Components::Transform,
            Components::Physics,
            Components::Thrusting,
            Components::Player>();
        const auto entity : view
        ) {
        auto& transform = view.get<Components::Transform>(entity);
        auto& physics = view.get<Components::Physics>(entity);
        auto& thrusting = view.get<Components::Thrusting>(entity);

        const float rotation_speed = 180.0f;
        if (IsKeyDown(KEY_RIGHT)) {
            transform.rotation += rotation_speed * dt;
        }
        if (IsKeyDown(KEY_LEFT)) {
            transform.rotation -= rotation_speed * dt;
        }

        // Normalize rotation
        if (transform.rotation >= 360.0f) {
            transform.rotation -= 360.0f;
        }
        if (transform.rotation < 0.0f) {
            transform.rotation += 360.0f;
        }

        if (IsKeyDown(KEY_UP)) {
            const float angle_radians = transform.rotation * DEG2RAD;
            const raylib::Vector2 thrust = {
                sin(angle_radians) * physics.acceleration * dt,
                -cos(angle_radians) * physics.acceleration * dt
            };

            physics.velocity += thrust;

            // Clamp speed to physics.max_speed
            const float speed_sq = physics.velocity.LengthSqr();
            float max_speed_sq = physics.max_speed * physics.max_speed;

            if (speed_sq > max_speed_sq) {
                physics.velocity = physics.velocity.Normalize() * physics.max_speed;
            }

            thrusting.active = true;
        } else {
            thrusting.active = false;
        }

        if (IsKeyDown(KEY_TAB)) {
            for (
                auto weapons = registry.view<
                    Components::Weapon,
                    Components::Transform,
                    Components::PlayerWeapon>();
                auto& weapon_entity : weapons
                ) {
                auto& weapon = weapons.get<Components::Weapon>(weapon_entity);
                auto&weapon_transform = weapons.get<Components::Transform>(weapon_entity);

                if (weapon.fire_timer.is_done()) {
                    spawn_bullet(registry, asset_manager, weapon_transform, physics, weapon);
                    weapon.fire_timer.start(weapon.cooldown);
                }
            };
        }
    };
}

void render_sprites(entt::registry& registry) {
    const auto view = registry.view<
        Components::Transform,
        Components::Renderable,
        Components::RenderOrder>
        (entt::exclude<Components::ShouldNotRender>);

    std::vector<entt::entity> sorted_entities;

    for (auto entity : view) {
        sorted_entities.push_back(entity);
    }

    std::ranges::sort(sorted_entities,
                      [&registry](const entt::entity a, const entt::entity b) {
                          return registry.get<Components::RenderOrder>(a).layer < registry.get<Components::RenderOrder>(b).layer;
                      }
    );

    for (const auto entity : sorted_entities) {
        const auto& [transform, renderable] = view.get<Components::Transform, Components::Renderable>(entity);

        const raylib::Rectangle source_rec = {
            0, 0,
            static_cast<float>(renderable.texture.width),
            static_cast<float>(renderable.texture.height)
        };
         const raylib::Rectangle dest_rec = {
            transform.position.x, transform.position.y,
            //transform.size.x, transform.size.y
            static_cast<float>(renderable.texture.width), static_cast<float>(renderable.texture.height)
        };
        const raylib::Vector2 origin = {renderable.texture.width/2.0f, renderable.texture.height/2.0f};
        renderable.texture.Draw(
            source_rec,
            dest_rec,
            origin,
            transform.rotation,
            renderable.color
        );
    }
}

entt::entity spawn_bullet(
    entt::registry& registry,
    AssetManager& asset_manager,
    Components::Transform& transform,
    const Components::Physics& physics,
    const Components::Weapon& weapon
) {
    const entt::entity bullet = registry.create();

    const float rotation_rad = transform.rotation * DEG2RAD;
    const raylib::Vector2 direction = {
        std::sin(rotation_rad),
        -std::cos(rotation_rad)
    };

    const raylib::Vector2 bullet_velocity = physics.velocity + (direction * weapon.shot_speed);

    Components::Physics bullet_physics = {weapon.shot_speed, 2'000'000, bullet_velocity};

    registry.emplace<Components::Physics>(bullet, bullet_physics);
    registry.emplace<Components::Transform>(bullet,
        transform.position,
        raylib::Vector2{5.0f, 5.0f},
        transform.rotation
    );

    Timer bullet_timer(weapon.lifetime);
    auto& bullet_component = registry.emplace<Components::Bullet>(bullet,
        50.0f,
        weapon.lifetime,
        bullet_timer
    );
    bullet_component.despawn_timer.start(weapon.lifetime);

    auto& renderable = registry.emplace<Components::Renderable>(bullet);
    renderable.texture = asset_manager.get_texture(weapon.munition);

    registry.emplace<Components::RenderOrder>(bullet, 10'000);

    return bullet;
}

entt::entity spawn_engine(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 relative_offset,
    entt::entity parent_ship
) {
    const auto entity = registry.create();

    // Handle stuff that depends on a found engine
    if (
        const auto engine = asset_manager.get_engine(key);
        !engine
    ) {
        std::println("Engine not found for: {}, minimal child will be spawned instead", key);
    } else {
        registry.emplace<Components::Engine>(entity, (*engine)->thrust);

        auto& renderable = registry.emplace<Components::Renderable>(entity);
        renderable.color = raylib::Color::White();
        renderable.texture = asset_manager.get_texture((*engine)->texture);

        registry.emplace<Components::RenderOrder>(entity, 999);

        registry.emplace<Components::ShouldNotRender>(entity);
    }

    // Do the rest of the stuff that can be done

    registry.emplace<Components::Transform>(entity,
        raylib::Vector2{0.0, 0.0}
    );

    registry.emplace<Components::RelativeTransform>(entity, relative_offset);

    registry.emplace<Components::Parent>(entity, parent_ship);

    return entity;
}

entt::entity spawn_player_ship(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position
) {
    const auto entity = registry.create();
    auto ship = asset_manager.get_ship(key);

    registry.emplace<Components::Player>(entity);

    registry.emplace<Components::Transform>(entity, position);

    auto& ship_physics = registry.emplace<Components::Physics>(entity);

    registry.emplace<Components::Thrusting>(entity, false);

    auto& renderable = registry.emplace<Components::Renderable>(entity);
    if (!ship) {
        std::println("Error loading ship: {}", ship.error());
    } else {
        renderable.texture = asset_manager.get_texture((*ship)->texture);
    }

    registry.emplace<Components::RenderOrder>(entity, 1000);

    if (!ship) {
        std::println("Ship not found for: {}, no weapons will be spawned", key);
    } else {
        for (const auto& weapon : (*ship)->weapons) {
            spawn_player_weapon(
                registry,
                asset_manager,
                weapon.weapon_type,
                raylib::Vector2{weapon.x, weapon.y},
                entity
            );
        }
    }

    if (!ship) {
        std::println("Ship not found for: {}, no engines will be spawned", key);
    } else {
        std::vector<float> engine_thrusts;
        for (const auto& engine : (*ship)->engines) {
            spawn_engine(
                registry,
                asset_manager,
                engine.engine_type,
                raylib::Vector2{engine.x, engine.y},
                entity
            );

            if (auto engine_result = asset_manager.get_engine(engine.engine_type);
                !engine_result) {
                std::println("Couldn't find engine_type: {} while constructing ship: {}", engine.engine_type, key);
            } else {
                engine_thrusts.push_back((*engine_result)->thrust);
            }
        }
        float sum = 0.0f;
        for (const auto& thrust : engine_thrusts) {
            sum += thrust;
        }
        ship_physics.acceleration = sum;
    }

    return entity;
}

entt::entity spawn_player_weapon(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    const raylib::Vector2 relative_offset,
    const entt::entity parent_ship
) {
    const auto weapon = spawn_weapon(registry, asset_manager, key, relative_offset, parent_ship);
    registry.emplace<Components::PlayerWeapon>(weapon);
    return weapon;
}

entt::entity spawn_ship(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position
) {
    const entt::entity entity = registry.create();
    auto ship = asset_manager.get_ship(key);

    registry.emplace<Components::Transform>(entity, position);

    registry.emplace<Components::Physics>(entity);

    registry.emplace<Components::Thrusting>(entity, false);

    auto& renderable = registry.emplace<Components::Renderable>(entity);
    if (!ship) {
        std::println("Error loading ship: {}", ship.error());
    } else {
        renderable.texture = asset_manager.get_texture((*ship)->texture);
    }

    registry.emplace<Components::RenderOrder>(entity, 0);

    if (!ship) {
        std::println("Ship not found for: {}, no weapons will be spawned.", key);
    } else {
        for (const auto& weapon : (*ship)->weapons) {
            spawn_weapon(
                registry,
                asset_manager,
                weapon.weapon_type,
                raylib::Vector2{weapon.x, weapon.y},
                entity
            );
        }
    }

    return entity;
}

entt::entity spawn_weapon(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 relative_offset,
    entt::entity parent_ship
) {
    const auto weapon_entity = registry.create();

    registry.emplace<Components::Transform>(weapon_entity,
        raylib::Vector2{0.0, 0.0}
    );

    registry.emplace<Components::RelativeTransform>(weapon_entity,
        relative_offset
    );

    registry.emplace<Components::Parent>(weapon_entity, parent_ship);

    auto& weapon_component = registry.emplace<Components::Weapon>(weapon_entity,
        key,
        asset_manager
    );
    weapon_component.trigger_cooldown();

    return weapon_entity;
}

void update_background_position(entt::registry &registry) {
    for (const auto player_view = registry.view<Components::Transform, Components::Player>();
        const auto player_entity : player_view) {
        const auto& player_transform = player_view.get<Components::Transform>(player_entity);

        for (auto background_view = registry.view<Components::Transform, Components::Background>();
            const auto background_entity : background_view) {
            auto& background_transform = background_view.get<Components::Transform>(background_entity);

            background_transform.position = player_transform.position;
        }
    }
}

void update_bullet_timers(
    entt::registry &registry,
    const float dt
) {
    for (const auto view = registry.view<Components::Bullet>();
        const auto entity : view) {
        auto& bullet = view.get<Components::Bullet>(entity);

        bullet.despawn_timer.update(dt);
    }
}

void update_local_transforms(entt::registry& registry) {
    for (const auto view = registry.view<Components::Transform, Components::RelativeTransform, Components::Parent>();
         const auto entity: view) {
        auto& transform = view.get<Components::Transform>(entity);
        const auto& relative = view.get<Components::RelativeTransform>(entity);
        const auto& parent_transform = registry.get<Components::Transform>(view.get<Components::Parent>(entity).parent);

        raylib::Vector2 rotated_offset = Vector2Rotate(relative.offset, (parent_transform.rotation * DEG2RAD));
        transform.position = parent_transform.position + rotated_offset;
        transform.rotation = parent_transform.rotation + relative.rotation;
    }
}

void update_physics_transforms(
    entt::registry& registry,
    float dt
) {
    const auto view = registry.view<Components::Transform, const Components::Physics>();
    view.each([&dt](auto& transform, const auto& physics) {
        transform.position += physics.velocity * dt;
    });
}

void update_weapon_timers(
    entt::registry& registry,
    float dt
) {
    for (
        const auto view = registry.view<Components::Weapon>();
        auto& entity : view
    ) {
        auto& weapon = view.get<Components::Weapon>(entity);

        weapon.fire_timer.update(dt);
    }
}
