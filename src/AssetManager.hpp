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
    float radius;

    explicit WeaponData(const json& j);
    explicit WeaponData(const pugi::xml_document& d);
};

struct EngineData {
    std::string texture;
    float thrust;
    float rotation;

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
    float max_speed;
    float radius;
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
        std::string affiliation;
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
    explicit MapData(const pugi::xml_document& d);
};

struct StartData {
    struct StartPlayerData {
        std::string ship_type;
        float x;
        float y;
    };

    std::string name;
    std::string initial_map;
    StartPlayerData player;

    explicit StartData(const json& j);
    explicit StartData(const pugi::xml_document& d);
};

struct AffiliationData {
    struct Relation {
        std::string faction;
        int32_t relation;
    };

    std::string name;
    std::vector<Relation> relations;

    explicit AffiliationData(const json& j);
    explicit AffiliationData(const pugi::xml_document& d);
};

class AssetManager {
public:
    ~AssetManager();

    void load_assets();

    [[nodiscard]]
    std::expected<const ShipData*, std::string> get_ship(const std::string& name) const;

    [[nodiscard]]
    std::expected<const WeaponData*, std::string> get_weapon(const std::string& name) const;

    [[nodiscard]]
    std::expected<const EngineData*, std::string> get_engine(const std::string& name) const;

    [[nodiscard]]
    std::expected<const MapData*, std::string> get_map(const std::string& name) const;

    [[nodiscard]]
    std::expected<const StartData*, std::string>get_start(const std::string& name) const;

    [[nodiscard]]
    raylib::TextureUnmanaged& get_texture(const std::string& name);

    [[nodiscard]]
    std::expected<const uint32_t, std::string>get_faction_id(const std::string& name) const;

    [[nodiscard]]
    std::expected<const int, std::string>get_relation(uint32_t base_faction, uint32_t sub_faction) const;

private:
    std::unordered_map<std::string, ShipData> m_ship_assets;
    std::unordered_map<std::string, WeaponData> m_weapon_assets;
    std::unordered_map<std::string, EngineData> m_engine_assets;
    std::unordered_map<std::string, MapData> m_map_assets;
    std::unordered_map<std::string, StartData> m_start_assets;
    std::vector<AffiliationData> m_raw_affiliations;
    std::unordered_map<std::string, int> m_faction_name_to_id;
    std::vector<std::string> m_faction_id_to_name;
    std::vector<std::vector<int>> m_relation_table;
    std::vector<raylib::TextureUnmanaged> m_textures;
    std::unordered_map<std::string, size_t> m_texture_map;

    static bool is_xml(const std::filesystem::directory_entry& entry);

    static bool is_json(const std::filesystem::directory_entry& entry);

    static bool is_png(const std::filesystem::directory_entry& entry);

    static bool is_texture_file(const std::filesystem::directory_entry& entry);

    static bool is_of_asset_type(const std::filesystem::directory_entry& entry, const std::string& asset_type);

    static std::string get_asset_name_from_filename(const std::filesystem::directory_entry& entry);

    static std::string get_texture_name(const std::filesystem::directory_entry& entry);

    void unload_all();

    static raylib::TextureUnmanaged& get_error_texture();
};
