/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "pch.h"

// Suppress warnings from external library (nlohmann/json)
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif

#include "./Game/DragonicTactics/External/json.hpp"

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <fstream>

struct MapData {
    std::string id;
    std::string name;
    int width;
    int height;
    std::vector<std::string> tiles;
    std::map<char, std::string> legend;
    std::map<std::string, Math::ivec2> spawn_points;
};

class MapDataRegistry : public CS230::Component {
public:
    void LoadMaps(const std::string& json_path);
    MapData GetMapData(const std::string& map_id) const;
    std::vector<std::string> GetAllMapIds() const;

private:
    std::map<std::string, MapData> maps_;
};