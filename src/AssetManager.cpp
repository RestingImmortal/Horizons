// Copyright 2025 RestingImmortal

#include "AssetManager.hpp"

#include <filesystem>
#include <format>
#include <fstream>

#include "Logger.hpp"

WeaponData::WeaponData(const json& j) {
    munition = j.value("munition", "");
    damage   = j.value("damage", 0.0f);
    lifetime = j.value("lifetime", 0.0f);
    cooldown = j.value("cooldown", 0.0f);
    radius   = j.value("radius", 0.0f);
}

WeaponData::WeaponData(const pugi::xml_document& d) {
    const auto root = d.child("WeaponData");
    munition = root.child("munition").text().as_string("");
    damage   = root.child("damage").text().as_float(0.0f);
    lifetime = root.child("lifetime").text().as_float(0.0f);
    cooldown = root.child("cooldown").text().as_float(0.0f);
    radius   = root.child("radius").text().as_float(0.0f);
}

EngineData::EngineData(const json& j) {
    texture  = j.value("texture", "");
    thrust   = j.value("thrust", 20.0f);
    rotation = j.value("rotation", 180.0f);
}

EngineData::EngineData(const pugi::xml_document& d) {
    const auto root = d.child("EngineData");
    texture  = root.child("texture").text().as_string("");
    thrust   = root.child("thrust").text().as_float(20.0f);
    rotation = root.child("rotation").text().as_float(180.0f);
}

ShipData::ShipData(const json& j) {
    texture   = j.value("texture", "");
    max_speed = j.value("max_speed", 400.0f);
    radius    = j.value("radius", 0.0f);
    for (const auto& item : j["weapons"]) {
        weapons.push_back({
            .weapon_type = item.at("type").get<std::string>(),
            .x = item.at("x").get<float>(),
            .y = item.at("y").get<float>()});
    }
    for (const auto& item : j["engines"]) {
        engines.push_back({
            .engine_type = item.at("type").get<std::string>(),
            .x = item.at("x").get<float>(),
            .y = item.at("y").get<float>()
        });
    }
}

ShipData::ShipData(const pugi::xml_document& d) {
    const auto root = d.child("ShipData");
    texture   = root.child("texture").text().as_string("");
    max_speed = root.child("max_speed").text().as_float(400.0f);
    radius    = root.child("radius").text().as_float(0.0f);
    for (pugi::xml_node node : root.children("weapons")) {
        weapons.push_back({
            .weapon_type = node.child("type").text().as_string(),
            .x = node.child("x").text().as_float(),
            .y = node.child("y").text().as_float()
        });
    }
    for (pugi::xml_node node : root.children("engines")) {
        engines.push_back({
            .engine_type = node.child("type").text().as_string(),
            .x = node.child("x").text().as_float(),
            .y = node.child("y").text().as_float()
        });
    }
}

MapData::MapData(const json& j) {
    metadata = { j.at("meta").at("name").get<std::string>() };

    for (const auto& item : j["backgrounds"]) {
        backgrounds.push_back({
            .image = item.at("image").get<std::string>(),
            .layer = item.at("layer").get<int>()
        });
    }
    for (const auto& item : j["ships"]) {
        ships.push_back({
            .ship_type = item.at("type").get<std::string>(),
            .x = item.at("x").get<float>(),
            .y = item.at("y").get<float>(),
            .affiliation = item.at("affiliation").get<std::string>()
        });
    }
    for (const auto& item : j["objects"]) {
        objects.push_back({
            .texture = item.at("texture").get<std::string>(),
            .x = item.at("x").get<float>(),
            .y = item.at("y").get<float>(),
            .layer = item.at("layer").get<int>()
        });
    }
}

MapData::MapData(const pugi::xml_document& d) {
    const auto root = d.child("MapData");
    metadata = { root.child("meta").child("name").text().as_string() };
    for (pugi::xml_node node : root.children("backgrounds")) {
        backgrounds.push_back({
            .image = node.child("image").text().as_string(),
            .layer = node.child("layer").text().as_int()
        });
    }
    for (pugi::xml_node node : root.children("ships")) {
        ships.push_back({
            .ship_type = node.child("type").text().as_string(),
            .x = node.child("x").text().as_float(),
            .y = node.child("y").text().as_float(),
            .affiliation = node.child("affiliation").text().as_string()
        });
    }
    for (pugi::xml_node node : root.children("objects")) {
        objects.push_back({
            .texture = node.child("texture").text().as_string(),
            .x = node.child("x").text().as_float(),
            .y = node.child("y").text().as_float(),
            .layer = node.child("layer").text().as_int()
        });
    }
}

StartData::StartData(const json &j) {
    name = j.at("name").get<std::string>();
    initial_map = j.at("initial map").get<std::string>();
    player = {
        .ship_type = j.at("player").at("ship_type").get<std::string>(),
        .x = j.at("player").at("x").get<float>(),
        .y = j.at("player").at("y").get<float>()
    };
}

StartData::StartData(const pugi::xml_document& d) {
    const auto root = d.child("StartData");
    name = root.child("name").text().as_string();
    initial_map = root.child("initial map").text().as_string();
    player = {
        .ship_type = root.child("player").child("ship_type").text().as_string(),
        .x = root.child("player").child("x").text().as_float(),
        .y = root.child("player").child("y").text().as_float()
    };
}

AffiliationData::AffiliationData(const json& j) {
    name = j.at("name").get<std::string>();
    for (auto relation : j["relations"]) {
        relations.push_back({
            .faction = relation.at("relation_name"),
            .relation = relation.at("relation")
        });
    }
}

AffiliationData::AffiliationData(const pugi::xml_document& d) {
    const auto root = d.child("AffiliationData");
    name = root.child("name").text().as_string();
    for (auto relation : root.children("relations")) {
        relations.push_back({
            .faction = relation.child("relation_name").text().as_string(),
            .relation = relation.child("relation").text().as_int()
        });
    }
}

// Public Methods

AssetManager::~AssetManager() {
    unload_all();
}

void AssetManager::load_assets() {
    unload_all();

    std::filesystem::path assets_dir = "./assets/";

    if (!std::filesystem::exists(assets_dir)) {
        H_CRITICAL("Asset Loading", "Assets directory not found!");
        throw std::runtime_error("Assets directory not found!");
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(assets_dir)) {
        if (is_of_asset_type(entry, "start")) {
            std::string start_name = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                std::string key = doc.child("StartData").child("name").text().as_string();
                m_start_assets.emplace(key, StartData(doc));
                H_INFO("Asset Loader", "Loaded Start {}: {}", start_name, key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    auto key = jsonData.at("name").get<std::string>();
                    m_start_assets.emplace(key, StartData(jsonData));
                    H_INFO("Asset Loader", "Loaded Start {}: {}", start_name, key);
                } catch (std::exception& e) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_of_asset_type(entry, "map")) {
            std::string map_name = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                std::string key = doc.child("MapData").child("meta").child("name").text().as_string();
                m_map_assets.emplace(key, MapData(doc));
                H_INFO("Asset Loader", "Loaded Map {}: {}", map_name, key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    auto key = jsonData.at("meta").at("name").get<std::string>();
                    m_map_assets.emplace(key, MapData(jsonData));
                    H_INFO("Asset Loader", "Loaded Map {}: {}", map_name, key);
                } catch (std::exception& e) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_of_asset_type(entry, "engine")) {
            std::string key = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                m_engine_assets.emplace(key, EngineData(doc));
                H_INFO("Asset Loader", "Loaded Engine: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_engine_assets.emplace(key, EngineData(jsonData));
                    H_INFO("Asset Loader", "Loaded Engine: {}", key);
                } catch (const std::exception& e) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_of_asset_type(entry, "weapon")) {
            std::string key = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                m_weapon_assets.emplace(key, WeaponData(doc));
                H_INFO("Asset Loader", "Loaded Weapon: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_weapon_assets.emplace(key, WeaponData(jsonData));
                    H_INFO("Asset Loader", "Loaded Weapon: {}", key);
                } catch (const std::exception& e) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_of_asset_type(entry, "ship")) {
            std::string key = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                m_ship_assets.emplace(key, ShipData(doc));
                H_INFO("Asset Loader", "Loaded Ship: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_ship_assets.emplace(key, ShipData(jsonData));
                    H_INFO("Asset Loader", "Loaded Ship: {}", key);
                } catch (const std::exception& e) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_of_asset_type(entry, "affiliation")) {
            std::string key = get_asset_name_from_filename(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (
                    pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result
                ) {
                    H_ERROR("Asset Loader", "Error loading {}: {}", entry.path().string(), result.description());
                    continue;
                }

                m_raw_affiliations.emplace_back(doc);
                H_INFO("Asset Loader", "Loaded Raw Affiliation: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_raw_affiliations.emplace_back(jsonData);
                    H_INFO("Asset Loader", "Loaded Raw Affiliation: {}", key);
                } catch (const std::exception& e) {
                    H_ERROR("AssetLoader", "Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_texture_file(entry)) {
            std::string name = get_texture_name(entry);
            m_textures.emplace_back(entry.path().string());
            m_texture_map[name] = m_textures.size() - 1;
            H_INFO("Asset Loader", "Loaded Texture: {}", name);
        }
    }

    // Post initial load processing

    std::size_t faction_count = m_raw_affiliations.size();

    // Map names to IDs
    m_faction_id_to_name.reserve(faction_count);
    m_faction_name_to_id.reserve(faction_count);

    for (std::size_t id = 0; id < faction_count; id++) {
        const auto& affiliation = m_raw_affiliations[id];

        m_faction_id_to_name.push_back(affiliation.name);
        m_faction_name_to_id.emplace(affiliation.name, id);

        H_INFO("Asset Loader", "Gave faction '{}' id {}", affiliation.name, id);
    }

    // Create NxN relation table initialized to 0
    m_relation_table.assign(faction_count, std::vector<int>(faction_count, 0));

    // Populate sparse relations
    for (std::size_t source_id = 0; source_id < faction_count; source_id++) {
        for (
            const auto& source_affiliation = m_raw_affiliations[source_id];
            const auto& relation_entry : source_affiliation.relations
        ) {
            const std::string& target_name = relation_entry.faction;

            auto target_iter = m_faction_name_to_id.find(target_name);
            if (target_iter == m_faction_name_to_id.end()) {
                throw std::runtime_error(std::format(
                    "Faction '{}' has relation toward unknown faction '{}'",
                    source_affiliation.name, target_name
                ));
            }

            std::size_t target_id = target_iter->second;
            m_relation_table[source_id][target_id] = relation_entry.relation;
        }
    }
}

std::expected<const ShipData*, std::string> AssetManager::get_ship(const std::string& name) const {
    if (const auto it = m_ship_assets.find(name); it != m_ship_assets.end()) {
        return &it->second;
    }
    return std::unexpected("Ship '" + name + "' not found");
}

std::expected<const WeaponData*, std::string> AssetManager::get_weapon(const std::string& name) const {
    if (const auto it = m_weapon_assets.find(name); it != m_weapon_assets.end()) {
        return &it->second;
    }
    return std::unexpected("Weapon '" + name + "' not found");
}

std::expected<const EngineData*, std::string> AssetManager::get_engine(const std::string& name) const {
    if (const auto it = m_engine_assets.find(name); it != m_engine_assets.end()) {
        return &it->second;
    }
    return std::unexpected("Engine '" + name + "' not found");
}

std::expected<const MapData*, std::string> AssetManager::get_map(const std::string& name) const {
    if (const auto it = m_map_assets.find(name); it != m_map_assets.end()) {
        return &it->second;
    }
    return std::unexpected("Map '" + name + "' not found");
}

std::expected<const StartData *, std::string> AssetManager::get_start(const std::string &name) const {
    if (const auto it = m_start_assets.find(name); it != m_start_assets.end()) {
        return &it->second;
    }
    return std::unexpected("Start '" + name + "' not found");
}

raylib::TextureUnmanaged& AssetManager::get_texture(const std::string& name) {
    if (const auto it = m_texture_map.find(name); it != m_texture_map.end()) {
        return m_textures[it->second];
    }
    H_ERROR("Asset Loader", "Could not find texture: {}", name);
    return get_error_texture();
}

std::expected<const uint32_t, std::string>AssetManager::get_faction_id(const std::string& name) const {
    if (const auto it = m_faction_name_to_id.find(name); it != m_faction_name_to_id.end()) {
        return it->second;
    }
    return std::unexpected("Affiliation '" + name +"' not assigned an id");
}

std::expected<const int, std::string> AssetManager::get_relation(const uint32_t base_faction, const uint32_t sub_faction) const {
    if (base_faction >= m_relation_table.size()) {
        return std::unexpected(
            std::format("Base Faction Index {} is out of bounds ({})", base_faction, m_relation_table.size())
        );
    }

    if (
        const auto& sub = m_relation_table[base_faction];
        sub_faction >= sub.size()
    ) {
        return std::unexpected(
            std::format("Sub Faction Index {} is out of bounds for Base {} ({})", sub_faction, base_faction, sub.size())
        );
    }

    return m_relation_table[base_faction][sub_faction];
}

// Private methods

bool AssetManager::is_xml(const std::filesystem::directory_entry& entry) {
    return entry.path().extension() == ".xml";
}

bool AssetManager::is_json(const std::filesystem::directory_entry& entry) {
    return entry.path().extension() == ".json";
}

bool AssetManager::is_png(const std::filesystem::directory_entry& entry) {
    return entry.path().extension() == ".png";
}

bool AssetManager::is_of_asset_type(const std::filesystem::directory_entry &entry, const std::string& asset_type) {
    return entry.is_regular_file() &&
        (is_xml(entry) || is_json(entry)) &&
        entry.path().stem().extension() == ('.' + asset_type);
}

bool AssetManager::is_texture_file(const std::filesystem::directory_entry& entry) {
    return entry.is_regular_file() && is_png(entry);
}

std::string AssetManager::get_asset_name_from_filename(const std::filesystem::directory_entry& entry) {
    return entry.path().stem().stem().string();
}

std::string AssetManager::get_texture_name(const std::filesystem::directory_entry& entry) {
    return entry.path().stem().string();
}

void AssetManager::unload_all() {
    for (auto& texture : m_textures) {
        texture.Unload();
    }
    m_textures.clear();
    m_texture_map.clear();
    m_ship_assets.clear();
    m_weapon_assets.clear();
    m_engine_assets.clear();
    m_map_assets.clear();
    m_start_assets.clear();
}

raylib::TextureUnmanaged& AssetManager::get_error_texture() {
    static raylib::TextureUnmanaged error_texture = []{
        const Image img = GenImageColor(128, 128, MAGENTA);
        const raylib::TextureUnmanaged tex = LoadTextureFromImage(img);
        UnloadImage(img);
        return tex;
    }();
    return error_texture;
}
