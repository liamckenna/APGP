#pragma once
#include "json.hpp"

std::unique_ptr<nlohmann::json> ReadJsonPointerFromFile(const std::string& filename);

nlohmann::json ReadJsonFromFile(const std::string& filename);

template <typename T>
nlohmann::json Fetch(const nlohmann::json& json, const std::string& key, const T& fallback) {
    if (json.contains(key)) return json[key];
    else return nlohmann::json(fallback);
}