#pragma once
#include "pch.h"
#include "./Game/DragonicTactics/External/json.hpp"
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