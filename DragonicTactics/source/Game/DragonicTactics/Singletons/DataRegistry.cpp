#include "DataRegistry.h"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Logger.hpp"
#include "../../../Engine/Path.hpp"
#include <fstream>
#include <sys/stat.h>  // For file timestamp

// ===== Basic Loading =====

void DataRegistry::LoadFromFile(const std::string& filepath)
{
    const std::filesystem::path json_path_ctor = assets::locate_asset(filepath);
    std::ifstream               file(json_path_ctor);
    
    if (!file.is_open())
    {
        Engine::GetLogger().LogError("DataRegistry: Can't open file: " + filepath);
        return;
    }

    try {
        nlohmann::json loaded_data;
        file >> loaded_data;
        file.close();
        
        // Merge with existing data
        data.merge_patch(loaded_data);
        
        // Track file timestamp for hot-reload
        fileTimestamps[filepath] = GetFileModifiedTime(filepath);
        
        Engine::GetLogger().LogEvent("DataRegistry: Loaded " + filepath);
    }
    catch (const std::exception&) {
        Engine::GetLogger().LogError("DataRegistry: JSON parse error in " + filepath);
        file.close();
    }
}

bool DataRegistry::HasKey(const std::string& key) const
{
    nlohmann::json value = FindValue(key);
    return !value.is_null();
}

nlohmann::json DataRegistry::FindValue(const std::string& key) const
{
    // Support 2-level keys: "Dragon.maxHP"
    
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos)
    {
        if (data.contains(key)) {
            return data[key];
        }
        return nlohmann::json();
    }

    std::string first = key.substr(0, dotPos);
    if (!data.contains(first)) {
        return nlohmann::json();
    }

    std::string second = key.substr(dotPos + 1);
    if (!data[first].contains(second)) {
        return nlohmann::json();
    }

    return data[first][second];
}

// ===== Hot Reload =====

void DataRegistry::ReloadFile(const std::string& filepath)
{
    Engine::GetLogger().LogEvent("DataRegistry: Reloading " + filepath);
    
    // Clear old data for this file
    // TODO: Track which keys came from which file
    
    LoadFromFile(filepath);
}

void DataRegistry::WatchFile(const std::string& filepath)
{
    // TODO: Implement file watching system
    // Store filepath in watch list
    // Check fileTimestamps on Update()
    
    Engine::GetLogger().LogDebug("DataRegistry: Watching " + filepath);
}

void DataRegistry::StopWatching(const std::string& filepath)
{
    // TODO: Remove from watch list
    
    Engine::GetLogger().LogDebug("DataRegistry: Stopped watching " + filepath);
}

long long DataRegistry::GetFileModifiedTime(const std::string& filepath) const
{
    struct stat fileInfo;
    if (stat(filepath.c_str(), &fileInfo) == 0) {
        return static_cast<long long>(fileInfo.st_mtime);
    }
    return 0;
}

// ===== Complex Data Access =====

nlohmann::json DataRegistry::GetJSON(const std::string& key) const
{
    return FindValue(key);
}

// ===== Debug & Validation =====

bool DataRegistry::ValidateSchema(const std::string& filepath)
{
    // TODO: Validate JSON structure
    // Check required keys exist
    // Check value types are correct
    
    Engine::GetLogger().LogDebug("DataRegistry: Validating " + filepath);
    return true;  // Placeholder
}

void DataRegistry::LogAllKeys() const
{
    Engine::GetLogger().LogDebug("========== DataRegistry Keys ==========");
    
    for (auto& [key, value] : data.items())
    {
        Engine::GetLogger().LogDebug("  " + key);
        
        if (value.is_object()) {
            for (auto& [subkey, subvalue] : value.items()) {
                Engine::GetLogger().LogDebug("    " + key + "." + subkey);
            }
        }
    }
}