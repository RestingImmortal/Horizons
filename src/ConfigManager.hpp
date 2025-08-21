// Copyright 2025 RestingImmortal

#pragma once

#include <nlohmann/json.hpp>

#include "Logger.hpp" // Don't use log() here, as it may be uninitialized and have no output.

struct ConfigManager {
    explicit ConfigManager();

    std::string title;
    LogLevel log_level;
};
