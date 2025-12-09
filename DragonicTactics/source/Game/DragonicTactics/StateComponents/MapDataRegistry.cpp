#include "MapDataRegistry.h"
#include "../../../Engine/Path.h"

using json = nlohmann::json;

void MapDataRegistry::LoadMaps(const std::string& json_path) {
    Engine::GetLogger().LogEvent("MapDataRegistry: Loading " + json_path);

    const std::filesystem::path full_path = assets::locate_asset(json_path);
    std::ifstream file(full_path);
    if (!file.is_open()) {
        Engine::GetLogger().LogError("Failed to open " + json_path);
        return;
    }

    json j;
    file >> j;

    for (const auto& map_json : j["maps"]) {
        MapData map_data;
        map_data.id = map_json["id"];
        map_data.name = map_json["name"];
        map_data.width = map_json["width"];
        map_data.height = map_json["height"];

        for (const auto& row : map_json["tiles"]) {
            map_data.tiles.push_back(row);
        }

        for (const auto& [key, value] : map_json["legend"].items()) {
            map_data.legend[key[0]] = value;
        }

        for (const auto& [char_type, pos] : map_json["spawn_points"].items()) {
            Math::ivec2 spawn_pos{pos["x"], pos["y"]};
            map_data.spawn_points[char_type] = spawn_pos;
        }

        maps_[map_data.id] = map_data;
        Engine::GetLogger().LogEvent("Loaded map: " + map_data.id);
    }
}

MapData MapDataRegistry::GetMapData(const std::string& map_id) const {
    auto it = maps_.find(map_id);
    if (it != maps_.end()) {
        return it->second;
    }

    Engine::GetLogger().LogError("Map not found: " + map_id);
    return MapData{};
}

std::vector<std::string> MapDataRegistry::GetAllMapIds() const
{
  std::vector<std::string> ids;
  for (const auto& [id, map_data] : maps_)
  {
    ids.push_back(id);
  }
  return ids;
}