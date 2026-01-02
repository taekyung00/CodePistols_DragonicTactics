/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./Engine/Component.h"

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

#include "./Game/DragonicTactics/Types/CharacterTypes.h"
#include <ctime>
#include <map>
#include <string>

class DataRegistry : public CS230::Component
{
  public:
  DataRegistry()  = default;
  ~DataRegistry() = default;

  // ===== Basic Loading =====

  void LoadFromFile(const std::string& filepath);
  // Load JSON file and merge into registry

  template <typename T>
  T GetValue(const std::string& key, const T& defaultValue) const;

  // Get typed value from registry with fallback

  bool HasKey(const std::string& key) const;
  // Check if key exists in registry

  // ===== Hot Reload =====

  void ReloadAll();
  // Reload All Json

  void ReloadCharacters();
  // Reload only characters

  void ReloadSpells();
  // Reload only spells

  // ===== Complex Data Access =====

  nlohmann::json GetJSON(const std::string& key) const;
  // Get raw JSON object for complex nested data

  template <typename T>
  std::vector<T> GetArray(const std::string& key) const;

  // Get array values from registry

  // ===== Week 4: Structured Data Access =====

  CharacterData GetCharacterData(const std::string& name);
  SpellData		GetSpellData(const std::string& name);

  // ===== Debug & Validation =====

  bool ValidateSchema(const std::string& filepath);
  // Check if JSON file has valid structure

  bool ValidateCharacterJSON(const std::string& filepath);
  // Validate character JSON structure

  bool ValidateSpellJSON(const std::string& filepath);
  // Validate spell JSON structure

  void LogAllKeys() const;
  // Debug: print all keys in registry

  bool LoadAllCharacterData(const std::string& filepath);

  private:

  nlohmann::json				   data;
  std::map<std::string, long long> fileTimestamps; // For hot-reload tracking

  nlohmann::json FindValue(const std::string& key) const;
  // Navigate nested JSON by dot-separated path (2 levels max for now)

  long long GetFileModifiedTime(const std::string& filepath) const;
  // Helper for hot-reload system

  // ===== Week 4: Data Storage =====
  std::map<std::string, CharacterData> characterDatabase;
  std::map<std::string, SpellData>	   spellDatabase;

  // ===== Week 4: Helper Functions =====
};

#include "DataRegistry.ini"