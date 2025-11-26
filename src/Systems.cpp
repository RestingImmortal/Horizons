// Copyright 2025 RestingImmortal

#include "Systems.hpp"

#include <cmath>
#include <print>

#include <raylib-cpp.hpp>

#include "AssetManager.hpp"
#include "Components.hpp"
#include "Logger.hpp"
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

void load_map(
    entt::registry &registry,
    AssetManager &asset_manager,
    const std::string &key
) {
    if (
        auto map_result = asset_manager.get_map(key);
        !map_result
    ) {
        H_WARNING("load_map", "{}: {}", key, map_result.error());
    } else {
        const auto map_data = *map_result;

        for (const auto& background : map_data->backgrounds) {
            spawn_background(
                registry,
                asset_manager,
                background.image,
                background.layer
            );
        }

        for (const auto& ship : map_data->ships) {
            if (
                auto affiliation_result = asset_manager.get_faction_id(ship.affiliation);
                !affiliation_result
            ) {
                H_WARNING("load_map", "{}: {}", key, affiliation_result.error());
            } else {
                spawn_ship(
                    registry,
                    asset_manager,
                    ship.ship_type,
                    raylib::Vector2{ship.x, ship.y},
                    *affiliation_result
                );
            }
        }

        for (const auto& object : map_data->objects) {
            spawn_object(
                registry,
                asset_manager,
                object.texture,
                raylib::Vector2{object.x, object.y},
                object.layer
            );
        }
    }
}

void load_start(
    entt::registry &registry,
    AssetManager &asset_manager,
    const std::string &key
) {
    if (
        auto start_result = asset_manager.get_start(key);
        !start_result
    ) {
        H_ERROR("load_start", "{}: {}", key, start_result.error());
    } else {
        load_map(
            registry,
            asset_manager,
            (*start_result)->initial_map
        );

        spawn_player_ship(
            registry,
            asset_manager,
            (*start_result)->player.ship_type,
            raylib::Vector2{(*start_result)->player.x, (*start_result)->player.y}
        );
    }
}

void mark_bullets_for_despawn(entt::registry &registry) {
    for (
        const auto view = registry.view<Components::Bullet>();
        const auto entity : view
    ) {
        if (auto& bullet = registry.get<Components::Bullet>(entity);
            bullet.despawn_timer.is_done()) {
            registry.emplace<Components::DespawnMarker>(entity);
        }
    }
}

void on_collision(
    entt::registry& registry,
    const AssetManager& asset_manager,
    const Events::Collision& event
) {
    if (const auto a_affiliation = registry.try_get<Components::Affiliation>(event.a)) {
        if (const auto b_affiliation = registry.try_get<Components::Affiliation>(event.b)) {
            if (
                const auto relation_result = asset_manager.get_relation(a_affiliation->id, b_affiliation->id);
                !relation_result
            ) {
                H_WARNING("Collision", "{}", relation_result.error());
            } else {
                if (
                    const auto relation_inverse_result = asset_manager.get_relation(b_affiliation->id, a_affiliation->id);
                    !relation_inverse_result
                ) {
                    H_WARNING("Collision", "{}", relation_inverse_result.error());
                } else {
                    if (*relation_result < 0 || *relation_inverse_result < 0) {
                        H_INFO("Collision", "With values of ({},{})", *relation_result, *relation_inverse_result);
                    }
                }
            }
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
            Components::Player,
            Components::Affiliation
        >();
        const auto entity : view
    ) {
        auto& transform = view.get<Components::Transform>(entity);
        auto& physics = view.get<Components::Physics>(entity);
        auto& thrusting = view.get<Components::Thrusting>(entity);
        auto& affiliation = view.get<Components::Affiliation>(entity);

        const float rotation_speed = physics.rotation;
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
                    spawn_bullet(registry, asset_manager, weapon_transform, physics, weapon, affiliation.id);
                    weapon.fire_timer.start(weapon.cooldown);
                }
            };
        }
    };
}

void print_engine(
    const AssetManager& asset_manager,
    const int indentation_level,
    const std::string& engine
) {
    if (
        const auto engine_result = asset_manager.get_engine(engine);
        !engine_result
    ) {
        H_DEBUG("Asset Printer", "Engine '{}' not found", engine);
    } else {
        const std::string indent(indentation_level, '-');

        std::println("{}Engine Texture: {}", indent, (*engine_result)->texture);
        std::println("{}Engine Thrust: {}", indent, (*engine_result)->thrust);
        std::println("{}Engine Rotation: {}", indent, (*engine_result)->rotation);
    }
}

void print_ship(
    const AssetManager& asset_manager,
    const int indentation_level,
    const std::string& ship
) {
    H_DEBUG("Asset Printer", "Printing out ship: {}", ship);

    if (
        const auto ship_result = asset_manager.get_ship(ship);
        !ship_result
    ) {
        H_DEBUG("Asset Printer", "Ship '{}' not found", ship);
    } else {
        const std::string indent(indentation_level, '-');

        std::println("{}Ship Texture: {}", indent, (*ship_result)->texture);
        std::println("{}Ship Max Speed: {}", indent, (*ship_result)->max_speed);
        std::println("{}Ship Radius: {}", indent, (*ship_result)->radius);
        for (const auto& weapon : (*ship_result)->weapons) {
            std::println("{}Weapon:", indent);
            std::println("{}-Weapon Type: {}", indent, weapon.weapon_type);
            std::println("{}-Weapon Coords: {}, {}", indent, weapon.x, weapon.y);
        }
        for (const auto& engine : (*ship_result)->engines) {
            std::println("{}Engine:", indent);
            std::println("{}-Engine Type: {}", indent, engine.engine_type);
            std::println("{}-Engine Coords: {}, {}", indent, engine.x, engine.y);
        }
    }
}

void print_ship_recurse(
    const AssetManager& asset_manager,
    const int indentation_level,
    const std::string& ship
) {
    H_DEBUG("Asset Printer", "Printing out ship: {}", ship);

    if (
        const auto ship_result = asset_manager.get_ship(ship);
        !ship_result
    ) {
        H_DEBUG("Asset Printer", "Ship '{}' not found", ship);
    } else {
        const std::string indent(indentation_level, '-');

        std::println("{}Ship Texture: {}", indent, (*ship_result)->texture);
        std::println("{}Ship Max Speed: {}", indent, (*ship_result)->max_speed);
        std::println("{}Ship Radius: {}", indent, (*ship_result)->radius);
        for (const auto& weapon : (*ship_result)->weapons) {
            std::println("{}Weapon:", indent);
            std::println("{}-Weapon Type: {}", indent, weapon.weapon_type);
            print_weapon(asset_manager, indentation_level + 2, weapon.weapon_type);
            std::println("{}-Weapon Coords: {}, {}", indent, weapon.x, weapon.y);
        }
        for (const auto& engine : (*ship_result)->engines) {
            std::println("{}Engine:", indent);
            std::println("{}-Engine Type: {}", indent, engine.engine_type);
            print_engine(asset_manager, indentation_level + 2, engine.engine_type);
            std::println("{}-Engine Coords: {}, {}", indent, engine.x, engine.y);
        }
    }
}

void print_weapon(
    const AssetManager& asset_manager,
    const int indentation_level,
    const std::string& weapon
) {
    if (
        const auto weapon_result = asset_manager.get_weapon(weapon);
        !weapon_result
    ) {
        H_DEBUG("Asset Printer", "Weapon '{}' not found", weapon);
    } else {
        const std::string indent(indentation_level, '-');

        std::println("{}Weapon Munition: {}", indent, (*weapon_result)->munition);
        std::println("{}Weapon Damage: {}", indent, (*weapon_result)->damage);
        std::println("{}Weapon Lifetime: {}", indent, (*weapon_result)->lifetime);
        std::println("{}Weapon Cooldown: {}", indent, (*weapon_result)->cooldown);
        std::println("{}Weapon radius: {}", indent, (*weapon_result)->radius);
    }
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

entt::entity spawn_background(
    entt::registry &registry,
    AssetManager &asset_manager,
    const std::string &key,
    int layer
) {
    const entt::entity entity = registry.create();

    registry.emplace<Components::Background>(entity);

    registry.emplace<Components::Transform>(entity);

    auto& bg_render = registry.emplace<Components::Renderable>(entity);
    bg_render.texture = asset_manager.get_texture(key);

    registry.emplace<Components::RenderOrder>(entity, layer);

    return entity;
}


entt::entity spawn_bullet(
    entt::registry& registry,
    AssetManager& asset_manager,
    Components::Transform& transform,
    const Components::Physics& physics,
    const Components::Weapon& weapon,
    const uint32_t affiliation
) {
    const entt::entity bullet = registry.create();

    registry.emplace<Components::Affiliation>(bullet, affiliation);

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

    registry.emplace<Components::Collider>(bullet,
        weapon.radius,
        0b10u,
        0b1u
    );

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
        H_WARNING("spawn_engine", "Engine not found for: {}, minimal child will be spawned instead", key);
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

entt::entity spawn_object(
    entt::registry& registry,
    AssetManager& asset_manager,
    const std::string& key,
    raylib::Vector2 position,
    int layer
) {
    const entt::entity entity = registry.create();

    registry.emplace<Components::Transform>(entity, position);

    auto& object_renderable = registry.emplace<Components::Renderable>(entity);
    object_renderable.texture = asset_manager.get_texture(key);

    registry.emplace<Components::RenderOrder>(entity, layer);

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

    if (!ship) {
        H_ERROR("spawn_player_ship", "{}", ship.error());
        H_WARNING("spawn_player_ship", "Minimal player ship will be spawned. Please consider resolving this.");
    } else {
        // If the ship isn't found, no texture will be found. Thus, don't give the entity a Renderable component.
        auto& renderable = registry.emplace<Components::Renderable>(entity);
        renderable.texture = asset_manager.get_texture((*ship)->texture);

        registry.emplace<Components::RenderOrder>(entity, 1000);

        registry.emplace<Components::Collider>(entity,
            (*ship)->radius,
            0b1u,
            0b10u
        );

        if (
            auto affiliation_result = asset_manager.get_faction_id("Player");
            ! affiliation_result
        ) {
            H_ERROR("spawn_player_ship", "Could not give player affiliation: {}", affiliation_result.error());
        } else {
            registry.emplace<Components::Affiliation>(entity, *affiliation_result);
        }

        // If the ship isn't found, there would be no weapons. Thus, only try spawning them when they might be present.
        for (const auto& weapon : (*ship)->weapons) {
            spawn_player_weapon(
                registry,
                asset_manager,
                weapon.weapon_type,
                raylib::Vector2{weapon.x, weapon.y},
                entity
            );
        }

        // Only do engine stuff when there might be engines.
        registry.emplace<Components::Thrusting>(entity, false);

        // Physics values depend on found data
        auto& ship_physics = registry.emplace<Components::Physics>(entity);
        ship_physics.max_speed = (*ship)->max_speed;

        std::vector<float> engine_thrusts;
        std::vector<float> engine_rotations;
        for (const auto& engine : (*ship)->engines) {
            spawn_engine(
                registry,
                asset_manager,
                engine.engine_type,
                raylib::Vector2{engine.x, engine.y},
                entity
            );

            if (
                auto engine_result = asset_manager.get_engine(engine.engine_type);
                !engine_result
            ) {
                H_WARNING("spawn_player_ship", "Couldn't find engine_type: {} while constructing ship: {}", engine.engine_type, key);
            } else {
                engine_thrusts.push_back((*engine_result)->thrust);
                engine_rotations.push_back((*engine_result)->rotation);
            }
        }
        float thrust_sum = 0.0f;
        for (const auto& thrust : engine_thrusts) {
            thrust_sum += thrust;
        }
        ship_physics.acceleration = thrust_sum;
        float rotation_sum = 0.0f;
        for (const auto& rotation : engine_rotations) {
            rotation_sum += rotation;
        }
        ship_physics.rotation = rotation_sum;
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
    raylib::Vector2 position,
    uint32_t affiliation
) {
    const entt::entity entity = registry.create();
    auto ship = asset_manager.get_ship(key);

    registry.emplace<Components::Transform>(entity, position);

    registry.emplace<Components::Physics>(entity);

    if (!ship) {
        H_ERROR("spawn_ship", "{}", ship.error());
        H_WARNING("spawn_ship", "Minimal ship entity will be spawned.");
    } else {
        // If the ship can't be found, there will be no texture found, and thus a renderable is useless
        auto& renderable = registry.emplace<Components::Renderable>(entity);
        renderable.texture = asset_manager.get_texture((*ship)->texture);

        registry.emplace<Components::RenderOrder>(entity, 0);

        registry.emplace<Components::Collider>(entity,
            (*ship)->radius,
            0b1u,
            0b10u
        );

        registry.emplace<Components::Affiliation>(entity, affiliation);

        // If the ship can't be found, there will be no engines found, and thus it doesn't need to bloat engine processing.
        registry.emplace<Components::Thrusting>(entity, false);

        // If the ship can't be found, there will be no weapons found, and thus engines shouldn't try to spawn unless the ship is present.
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
    for (
        const auto player_view = registry.view<Components::Transform, Components::Player>();
        const auto player_entity : player_view
    ) {
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
    for (
        const auto view = registry.view<Components::Bullet>();
        const auto entity : view
    ) {
        auto& bullet = view.get<Components::Bullet>(entity);

        bullet.despawn_timer.update(dt);
    }
}

void update_collision(
    entt::registry& registry,
    entt::dispatcher& dispatcher
) {
    const auto view = registry.view<Components::Transform, Components::Collider>();

    for (auto it_a = view.begin(); it_a != view.end(); ++it_a) {
        const auto entity_a = *it_a;
        const auto& transform_a = view.get<Components::Transform>(entity_a);
        const auto& collider_a  = view.get<Components::Collider>(entity_a);

        for (auto it_b = std::next(it_a); it_b != view.end(); ++it_b) {
            const auto entity_b = *it_b;
            const auto& transform_b = view.get<Components::Transform>(entity_b);
            const auto& collider_b  = view.get<Components::Collider>(entity_b);

            if (
                !((collider_a.collides_with & collider_b.category) ||
                    (collider_b.collides_with & collider_a.category))
            ) {
                continue;
            }

            const raylib::Vector2 delta = transform_b.position - transform_a.position;

            const float dist_sq = delta.x * delta.x + delta.y * delta.y;
            const float radius_sum = collider_a.radius + collider_b.radius;

            if (dist_sq <= radius_sum * radius_sum) {
                dispatcher.enqueue<Events::Collision>(entity_a, entity_b);
            }
        }
    }
}

void update_local_transforms(entt::registry& registry) {
    for (
        const auto view = registry.view<Components::Transform, Components::RelativeTransform, Components::Parent>();
        const auto entity: view
    ) {
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
