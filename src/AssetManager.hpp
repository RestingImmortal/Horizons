// Copyright 2025 RestingImmortal

#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <print>
#include <unordered_map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <pugixml.hpp>
#include <raylib-cpp.hpp>

using json = nlohmann::json;

struct WeaponData {
    std::string munition;
    float damage;
    float lifetime;
    float cooldown;

    explicit WeaponData(const json& j);
    explicit WeaponData(const pugi::xml_document& d);
};

struct EngineData {
    std::string texture;

    explicit EngineData(const json& j);
    explicit EngineData(const pugi::xml_document& d);
};

struct ShipEngineData {
    std::string engine_type;
    float x;
    float y;
};

struct ShipWeaponData {
    std::string weapon_type;
    float x;
    float y;
};

struct ShipData {
    std::string texture;
    std::vector<ShipWeaponData> weapons;
    std::vector<ShipEngineData> engines;
    
    explicit ShipData(const json& j);
    explicit ShipData(const pugi::xml_document& d);
};

class AssetManager {
public:
    ~AssetManager();

    void load_assets();

    std::expected<const ShipData*, std::string> get_ship(const std::string& name) const;

    std::expected<const WeaponData*, std::string> get_weapon(const std::string& name) const;

    std::expected<const EngineData*, std::string> get_engine(const std::string& name) const;

    raylib::TextureUnmanaged& get_texture(const std::string& name);

private:
    std::unordered_map<std::string, ShipData> m_ship_assets;
    std::unordered_map<std::string, WeaponData> m_weapon_assets;
    std::unordered_map<std::string, EngineData> m_engine_assets;
    std::vector<raylib::TextureUnmanaged> m_textures;
    std::unordered_map<std::string, size_t> m_texture_map;

    bool is_xml(const std::filesystem::directory_entry& entry);

    bool is_json(const std::filesystem::directory_entry& entry);

    bool is_png(const std::filesystem::directory_entry& entry);

    bool is_ship_file(const std::filesystem::directory_entry& entry);

    bool is_weapon_file(const std::filesystem::directory_entry& entry);

    bool is_engine_file(const std::filesystem::directory_entry& entry);

    bool is_texture_file(const std::filesystem::directory_entry& entry);

    static std::string get_ship_name(const std::filesystem::directory_entry& entry);

    static std::string get_weapon_name(const std::filesystem::directory_entry& entry);

    static std::string get_engine_name(const std::filesystem::directory_entry& entry);

    static std::string get_texture_name(const std::filesystem::directory_entry& entry);

    void unload_all();

    static raylib::TextureUnmanaged& get_error_texture();
};
