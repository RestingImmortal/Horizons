// Copyright 2025 RestingImmortal

#include "AssetManager.hpp"

#include <filesystem>
#include <fstream>

WeaponData::WeaponData(const json& j) {
    munition = j.value("munition", "");
    damage   = j.value("damage", 0.0f);
    lifetime = j.value("lifetime", 0.0f);
    cooldown = j.value("cooldown", 0.0f);
}

WeaponData::WeaponData(const pugi::xml_document& d) {
    const auto root = d.child("WeaponData");
    munition = root.child("munition").text().as_string("");
    damage   = root.child("damage").text().as_float(0.0f);
    lifetime = root.child("lifetime").text().as_float(0.0f);
    cooldown = root.child("cooldown").text().as_float(0.0f);
}

EngineData::EngineData(const json& j) {
    texture = j.value("texture", "");
    thrust  = j.value("thrust", 20.0f);
}

EngineData::EngineData(const pugi::xml_document& d) {
    const auto root = d.child("EngineData");
    texture = root.child("texture").text().as_string("");
    thrust  = root.child("thrust").text().as_float(20.0f);
}

ShipData::ShipData(const json& j) {
    texture = j.value("texture", "");
    for (const auto& item : j["weapons"]) {
        weapons.push_back({
            item.at("type").get<std::string>(),
            item.at("x").get<float>(),
            item.at("y").get<float>()});
    }
    for (const auto& item : j["engines"]) {
        engines.push_back({
            item.at("type").get<std::string>(),
            item.at("x").get<float>(),
            item.at("y").get<float>()
        });
    }
}

ShipData::ShipData(const pugi::xml_document& d) {
    const auto root = d.child("ShipData");
    texture = root.child("texture").text().as_string("");
    for (pugi::xml_node node : root.children("weapons")) {
        weapons.push_back({
            node.child("type").text().as_string(),
            node.child("x").text().as_float(),
            node.child("y").text().as_float()
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
        std::println("Assets directory not found!");
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(assets_dir)) {
        if (is_engine_file(entry)) {
            std::string key = get_engine_name(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result) {
                    std::println("XML [{}] parsed with errors", entry.path().string());
                    std::println("Error: {}", result.description());
                    continue;
                }

                m_engine_assets.emplace(key, EngineData(doc));
                std::println("Loaded Engine: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_engine_assets.emplace(key, EngineData(jsonData));
                    std::println("Loaded Engine: {}", key);
                } catch (const std::exception& e) {
                    std::println("Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_weapon_file(entry)) {
            std::string key = get_weapon_name(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result) {
                    std::println("XML [{}] parsed with errors", entry.path().string());
                    std::println("Error: {}", result.description());
                    continue;
                }

                m_weapon_assets.emplace(key, WeaponData(doc));
                 std::println("Loaded Weapon: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_weapon_assets.emplace(key, WeaponData(jsonData));
                    std::println("Loaded Weapon: {}", key);
                } catch (const std::exception& e) {
                    std::println("Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_ship_file(entry)) {
            std::string key = get_ship_name(entry);

            if (is_xml(entry)) {
                pugi::xml_document doc;

                if (pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
                    !result) {
                    std::println("XML [{}] parsed with errors", entry.path().string());
                    std::println("Error: {}", result.description());
                    continue;
                }

                m_ship_assets.emplace(key, ShipData(doc));
                std::println("Loaded Ship: {}", key);
            } else if (is_json(entry)) {
                try {
                    std::ifstream file(entry.path());
                    json jsonData = json::parse(file);
                    m_ship_assets.emplace(key, ShipData(jsonData));
                    std::println("Loaded Ship: {}", key);
                } catch (const std::exception& e) {
                    std::println("Error loading {}: {}", entry.path().string(), e.what());
                }
            }
        } else if (is_texture_file(entry)) {
            std::string name = get_texture_name(entry);
            m_textures.emplace_back(entry.path().string());
            m_texture_map[name] = m_textures.size() - 1;
            std::println("Loaded Texture: {}", name);
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

raylib::TextureUnmanaged& AssetManager::get_texture(const std::string& name) {
    if (const auto it = m_texture_map.find(name); it != m_texture_map.end()) {
        return m_textures[it->second];
    }
    std::println("Error getting texture {}", name);
    return get_error_texture();
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

bool AssetManager::is_ship_file(const std::filesystem::directory_entry& entry) {
    return entry.is_regular_file() &&
           (is_xml(entry) || is_json(entry)) &&
           entry.path().stem().extension() == ".ship";
}

bool AssetManager::is_weapon_file(const std::filesystem::directory_entry& entry) {
    return entry.is_regular_file() &&
           (is_xml(entry) || is_json(entry)) &&
           entry.path().stem().extension() == ".weapon";
}

bool AssetManager::is_engine_file(const std::filesystem::directory_entry& entry) {
    return entry.is_regular_file() &&
           (is_xml(entry) || is_json(entry)) &&
           entry.path().stem().extension() == ".engine";
}

bool AssetManager::is_texture_file(const std::filesystem::directory_entry& entry) {
    return entry.is_regular_file() && is_png(entry);
}

std::string AssetManager::get_ship_name(const std::filesystem::directory_entry& entry) {
    return entry.path().stem().stem().string();
}

std::string AssetManager::get_weapon_name(const std::filesystem::directory_entry& entry) {
    return entry.path().stem().stem().string();
}

std::string AssetManager::get_engine_name(const std::filesystem::directory_entry& entry) {
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
