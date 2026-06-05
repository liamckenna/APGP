#pragma once
#include "json.hpp"
#include <glm/glm.hpp>

namespace nlohmann {
    template <>
    struct adl_serializer<glm::vec3> {
        static glm::vec3 from_json(const json& j) {
            return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
        }
        static void to_json(json& j, const glm::vec3& v) {
            j = { v.x, v.y, v.z };
        }
    };
}

std::unique_ptr<nlohmann::json> ReadJsonPointerFromFile(const std::string& filename);

nlohmann::json ReadJsonFromFile(const std::string& filename);

template <typename T>
nlohmann::json Fetch(const nlohmann::json& json, const std::string& key, const T& fallback) {
    if (json.contains(key)) return json[key];
    else return nlohmann::json(fallback);
}

template <typename T>
glm::vec3 FetchVec3(const nlohmann::json& json, const std::string& key, const T& fallback) {
    if (json.contains(key)) return json[key];
    else return nlohmann::json(fallback);
}