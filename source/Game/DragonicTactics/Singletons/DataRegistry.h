#pragma once
#include <string>
#include <map>
#include "../../../External/json.hpp"

class DataRegistry {
public:
    // Singleton access
    static DataRegistry& Instance() {
        static DataRegistry instance;
        return instance;
    }

    DataRegistry(const DataRegistry&) = delete;
    DataRegistry& operator=(const DataRegistry&) = delete;

    // ===== Basic Loading =====
    
    void LoadFromFile(const std::string& filepath);
    // Load JSON file and merge into registry

    template<typename T>
    T GetValue(const std::string& key, const T& defaultValue) const {
        nlohmann::json value = FindValue(key);
        if (value.is_null()) {
            return defaultValue;
        }
        
        try {
            return value.get<T>();
        }
        catch (const std::exception&) {
            return defaultValue;
        }
    }
    // Get typed value from registry with fallback

    bool HasKey(const std::string& key) const;
    // Check if key exists in registry

    // ===== Hot Reload =====
    
    void ReloadFile(const std::string& filepath);
    // Hot-reload file at runtime for live balance tuning

    void WatchFile(const std::string& filepath);
    // Start watching file for changes (auto-reload)

    void StopWatching(const std::string& filepath);
    // Stop watching file

    // ===== Complex Data Access =====
    
    nlohmann::json GetJSON(const std::string& key) const;
    // Get raw JSON object for complex nested data

    template<typename T>
    std::vector<T> GetArray(const std::string& key) const {
        nlohmann::json value = FindValue(key);
        if (value.is_null() || !value.is_array()) {
            return std::vector<T>();
        }
        
        try {
            return value.get<std::vector<T>>();
        }
        catch (const std::exception&) {
            return std::vector<T>();
        }
    }
    // Get array values from registry

    // ===== Debug & Validation =====
    
    bool ValidateSchema(const std::string& filepath);
    // Check if JSON file has valid structure

    void LogAllKeys() const;
    // Debug: print all keys in registry

private:
    DataRegistry() = default;
    
    nlohmann::json data;
    std::map<std::string, long long> fileTimestamps;  // For hot-reload tracking
    
    nlohmann::json FindValue(const std::string& key) const;
    // Navigate nested JSON by dot-separated path (2 levels max for now)
    
    long long GetFileModifiedTime(const std::string& filepath) const;
    // Helper for hot-reload system
};