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
    float thrust;

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

struct MapData {
    struct MapMeta {
        std::string name;
    };

    struct BackgroundMapData {
        std::string image;
        int layer;
    };

    struct ShipMapData {
        std::string ship_type;
        float x;
        float y;
    };

    struct ObjectMapData {
        std::string texture;
        float x;
        float y;
        int layer;
    };

    MapMeta metadata;
    std::vector<BackgroundMapData> backgrounds;
    std::vector<ShipMapData> ships;
    std::vector<ObjectMapData> objects;

    explicit MapData(const json& j);
};

class AssetManager {
public:
    ~AssetManager();

    void load_assets();

    std::expected<const ShipData*, std::string> get_ship(const std::string& name) const;

    std::expected<const WeaponData*, std::string> get_weapon(const std::string& name) const;

    std::expected<const EngineData*, std::string> get_engine(const std::string& name) const;

    std::expected<const MapData*, std::string> get_map(const std::string& name) const;

    raylib::TextureUnmanaged& get_texture(const std::string& name);

private:
    std::unordered_map<std::string, ShipData> m_ship_assets;
    std::unordered_map<std::string, WeaponData> m_weapon_assets;
    std::unordered_map<std::string, EngineData> m_engine_assets;
    std::unordered_map<std::string, MapData> m_map_assets;
    std::vector<raylib::TextureUnmanaged> m_textures;
    std::unordered_map<std::string, size_t> m_texture_map;

    static bool is_xml(const std::filesystem::directory_entry& entry);

    static bool is_json(const std::filesystem::directory_entry& entry);

    static bool is_png(const std::filesystem::directory_entry& entry);

    static bool is_ship_file(const std::filesystem::directory_entry& entry);

    static bool is_weapon_file(const std::filesystem::directory_entry& entry);

    static bool is_engine_file(const std::filesystem::directory_entry& entry);

    static bool is_texture_file(const std::filesystem::directory_entry& entry);

    static bool is_map_file(const std::filesystem::directory_entry& entry);

    static std::string get_ship_name(const std::filesystem::directory_entry& entry);

    static std::string get_weapon_name(const std::filesystem::directory_entry& entry);

    static std::string get_engine_name(const std::filesystem::directory_entry& entry);

    static std::string get_map_name(const std::filesystem::directory_entry& entry);

    static std::string get_texture_name(const std::filesystem::directory_entry& entry);

    void unload_all();

    static raylib::TextureUnmanaged& get_error_texture();
};
