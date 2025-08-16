// Copyright 2025 RestingImmortal

#include "Components.hpp"
#include "AssetManager.hpp"

using namespace Components;

Weapon::Weapon(std::string key, const AssetManager& asset_manager)
    : damage(0.0), lifetime(0.0), cooldown(2'000'000) {
    if (const auto result = asset_manager.get_weapon(key);
        !result) {
        std::println("Error constructing weapon '{}', constructing default.", key);
    } else {
        const auto data = *result;
        munition = data->munition;
        damage   = data->damage;
        lifetime = data->lifetime;
        cooldown = data->cooldown;
    }
}

bool Weapon::can_fire() const {
    return !fire_timer.is_active() || fire_timer.is_done();
}

void Weapon::trigger_cooldown() {
    fire_timer.start(cooldown);
}
