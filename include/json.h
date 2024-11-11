#pragma once
#include "json.hpp"

std::unique_ptr<nlohmann::json> ReadJsonPointerFromFile(const std::string& filename);

nlohmann::json ReadJsonFromFile(const std::string& filename);