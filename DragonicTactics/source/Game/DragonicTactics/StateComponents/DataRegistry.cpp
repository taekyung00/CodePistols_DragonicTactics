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
    // Support 2-level keys: "Dragon.max_hp"
    
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

void DataRegistry::ReloadAll() {
    Engine::GetLogger().LogEvent("=== RELOADING ALL DATA ===");
    
    bool anyReloaded = false;
    
    // characters.json maybe change
    std::string charactersFile = "Data/characters.json";
    
    //checking when edit
    long long currentModTime = GetFileModifiedTime(charactersFile);
    long long lastModTime = fileTimestamps[charactersFile];
    
    //when file load first, or edit
    if (currentModTime > lastModTime) {
        Engine::GetLogger().LogEvent("Reloading " + charactersFile + " (modified)");
        
        if (LoadAllCharacterData(charactersFile)) {
            fileTimestamps[charactersFile] = currentModTime;
            anyReloaded = true;
            Engine::GetLogger().LogEvent("All character data reloaded");
        } else {
            Engine::GetLogger().LogError("Failed to reload character data");
        }
    }
    
    // TODO: after spells.json
    
    if (anyReloaded) {
        Engine::GetLogger().LogEvent("Data reload complete - systems notified");
    } else {
        Engine::GetLogger().LogEvent("No files modified - reload skipped");
    }
}

void DataRegistry::ReloadCharacters() {
    Engine::GetLogger().LogEvent("=== RELOADING CHARACTER DATA ===");
    
    std::string charactersFile = "Data/characters.json";
    
    long long currentModTime = GetFileModifiedTime(charactersFile);
    long long lastModTime = fileTimestamps[charactersFile];
    
    if (currentModTime > lastModTime) {
        Engine::GetLogger().LogEvent("Reloading " + charactersFile);
        
        if (LoadAllCharacterData(charactersFile)) {
            fileTimestamps[charactersFile] = currentModTime;
            Engine::GetLogger().LogEvent("Character reload complete");
        } else {
            Engine::GetLogger().LogError("Character reload failed");
        }
    } else {
        Engine::GetLogger().LogEvent("No character file changes detected");
    }
}

void DataRegistry::ReloadSpells() {
    Engine::GetLogger().LogEvent("=== RELOADING SPELL DATA ===");
    Engine::GetLogger().LogEvent("Spell data reload - not implemented yet (Week 4 TODO)");
}

long long DataRegistry::GetFileModifiedTime(const std::string& filepath) const
{
    struct stat fileInfo;
    if (stat(filepath.c_str(), &fileInfo) == 0) {
        return static_cast<long long>(fileInfo.st_mtime);
    }
    return 0;
}

// ===== Week 4: Helper Functions =====

bool DataRegistry::LoadAllCharacterData(const std::string& filepath) {
    const std::filesystem::path json_path = assets::locate_asset(filepath);
    std::ifstream file(json_path);
    
    if (!file.is_open()) {
        Engine::GetLogger().LogError("Failed to open: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json doc;
        file >> doc;
        file.close();
        
        if (!doc.is_object()) {
            Engine::GetLogger().LogError("Root must be object in " + filepath);
            return false;
        }
        
        //character
        for (auto& [characterName, charData] : doc.items()) {
            CharacterData Cdata;
            Cdata.character_type = characterName;
            
            //necessary stuff
            if (!charData.contains("max_hp") || !charData["max_hp"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'max_hp'");
                continue;
            }
            Cdata.max_hp = charData["max_hp"].get<int>();
            
            if (!charData.contains("speed") || !charData["speed"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'speed'");
                continue;
            }
            Cdata.speed = charData["speed"].get<int>();
            
            if (!charData.contains("max_action_points") || !charData["max_action_points"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'max_action_points'");
                continue;
            }
            Cdata.max_action_points = charData["max_action_points"].get<int>();
            
            if (!charData.contains("base_attack_power") || !charData["base_attack_power"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'base_attack_power'");
                continue;
            }
            Cdata.base_attack_power = charData["base_attack_power"].get<int>();
            
            if (!charData.contains("attack_dice") || !charData["attack_dice"].is_string()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'attack_dice'");
                continue;
            }
            Cdata.attack_dice = charData["attack_dice"].get<std::string>();
            
            if (!charData.contains("base_defense_power") || !charData["base_defense_power"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'base_defense_power'");
                continue;
            }
            Cdata.base_defense_power = charData["base_defense_power"].get<int>();
            
            if (!charData.contains("defense_dice") || !charData["defense_dice"].is_string()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'defense_dice'");
                continue;
            }
            Cdata.defense_dice = charData["defense_dice"].get<std::string>();
            
            if (!charData.contains("attack_range") || !charData["attack_range"].is_number_integer()) {
                Engine::GetLogger().LogError(characterName + ": Missing or invalid 'attack_range'");
                continue;
            }
            Cdata.attack_range = charData["attack_range"].get<int>();
            
            // spell_slots optional
            if (charData.contains("spell_slots") && charData["spell_slots"].is_object()) {
                for (auto& [levelStr, count] : charData["spell_slots"].items()) {
                    int level = std::stoi(levelStr);
                    Cdata.spell_slots[level] = count.get<int>();
                }
            }
            
            // known_spells optional
            if (charData.contains("known_spells") && charData["known_spells"].is_array()) {
                Cdata.known_spells = charData["known_spells"].get<std::vector<std::string>>();
            }
            
            // known_abilities optional
            if (charData.contains("known_abilities") && charData["known_abilities"].is_array()) {
                Cdata.known_abilities = charData["known_abilities"].get<std::vector<std::string>>();
            }
            
            //save to database
            characterDatabase[characterName] = Cdata;
            
            Engine::GetLogger().LogEvent("Loaded " + characterName + 
                ": HP=" + std::to_string(Cdata.max_hp) + 
                ", Speed=" + std::to_string(Cdata.speed));
        }
        
        return true;
    }
    catch (const std::exception& e) {
        Engine::GetLogger().LogError("JSON parse error in " + filepath + ": " + e.what());
        file.close();
        return false;
    }
}

// ===== Week 4: Structured Data Access =====

CharacterData DataRegistry::GetCharacterData(const std::string& name) {
    auto it = characterDatabase.find(name);
    if (it != characterDatabase.end()) {
        return it->second;
    }
    
    Engine::GetLogger().LogError("Character data not found: " + name);
    return CharacterData{};
}

SpellData DataRegistry::GetSpellData(const std::string& name) {
    auto it = spellDatabase.find(name);
    if (it != spellDatabase.end()) {
        return it->second;
    }
    
    Engine::GetLogger().LogError("Spell data not found: " + name);
    return SpellData{};
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

bool DataRegistry::ValidateCharacterJSON(const std::string& filepath) {
    Engine::GetLogger().LogEvent("Validating: " + filepath);
    
    //load and check
    bool result = LoadAllCharacterData(filepath);
    
    if (result) {
        Engine::GetLogger().LogEvent("Validation passed: " + filepath);
    } else {
        Engine::GetLogger().LogError("Validation failed: " + filepath);
    }
    
    return result;
}

bool DataRegistry::ValidateSpellJSON(const std::string& filepath) {
    (void)filepath;
    //after SpellJson created, this funtion will be change
    Engine::GetLogger().LogEvent("Spell validation - not implemented yet (Week 4 TODO)");
    return true;
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