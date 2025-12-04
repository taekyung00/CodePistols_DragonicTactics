/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  CharacterFactory.cpp
Project:    DragonicTactics
Author:     ginam
Created:    November 25, 2025
Updated:    November 25, 2025
*/

#include "CharacterFactory.h"
#include "../Objects/Components/ActionPoints.h"
#include "../Objects/Components/SpellSlots.h"
#include "../Objects/Components/StatsComponent.h"
#include "../Objects/Dragon.h"
#include "../Objects/Fighter.h"
#include "../StateComponents/DataRegistry.h"
#include "Engine/Engine.h"
#include "Engine/GameStateManager.h"
#include "Engine/Logger.h"

// Developer D: Helper function to convert CharacterData (JSON) to CharacterStats (game)
CharacterStats ConvertToCharacterStats(const CharacterData& data)
{
	CharacterStats stats;
	stats.max_hp	   = data.max_hp;
	stats.current_hp   = data.max_hp; // Start at full HP
	stats.base_attack  = data.base_attack_power;
	stats.attack_dice  = data.attack_dice;
	stats.base_defend  = data.base_defense_power;
	stats.defend_dice  = data.defense_dice;
	stats.speed		   = data.speed;
	stats.attack_range = data.attack_range;
	return stats;
}

// Developer D: Main factory method - creates character based on type
std::unique_ptr<Character> CharacterFactory::Create(CharacterTypes type, Math::ivec2 start_position)
{
	switch (type)
	{
		case CharacterTypes::Dragon: return CreateDragon(start_position);

		case CharacterTypes::Fighter:
			return CreateFighter(start_position);

			// TODO: Week 5+ - Add more character types
			// case CharacterTypes::Cleric:
			//     return CreateCleric(start_position);
			// case CharacterTypes::Wizard:
			//     return CreateWizard(start_position);
			// case CharacterTypes::Rogue:
			//     return CreateRogue(start_position);

		case CharacterTypes::None:
		case CharacterTypes::Count:
		default: Engine::GetLogger().LogError("CharacterFactory::Create - Unknown character type: " + std::to_string(static_cast<int>(type))); return nullptr;
	}
}

// Developer D: Factory method with custom stats (for future DataRegistry integration)
 std::unique_ptr<Character> CharacterFactory::CreateWithStats(CharacterTypes type, Math::ivec2 start_position, const CharacterStats& custom_stats)
{
	std::unique_ptr<Character> character = Create(type, start_position);

	if (character != nullptr)
	{
		StatsComponent* stats_comp = character->GetStatsComponent();
		if (stats_comp != nullptr)
		{
			*stats_comp = StatsComponent(custom_stats);
			Engine::GetLogger().LogDebug("CharacterFactory::CreateWithStats - Custom stats applied");
		}
	}

	return character;
}

// Developer D: Dragon-specific creation with JSON data loading
 std::unique_ptr<Dragon> CharacterFactory::CreateDragon(Math::ivec2 position)
{
	std::unique_ptr<Dragon> dragon = std::make_unique<Dragon>(position);

	// Developer D: Load CharacterData from DataRegistry JSON
	DataRegistry* registry = Engine::GetGameStateManager().GetGSComponent<DataRegistry>();
	if (registry != nullptr)
	{
		CharacterData data = registry->GetCharacterData("Dragon");

		// Developer D: Convert CharacterData (JSON) to CharacterStats (game)
		CharacterStats stats = ConvertToCharacterStats(data);

		// Apply stats to StatsComponent
		StatsComponent* stats_comp = dragon->GetStatsComponent();
		if (stats_comp != nullptr)
		{
			*stats_comp = StatsComponent(stats);
		}

		// Apply action points from CharacterData
		ActionPoints* ap = dragon->GetActionPointsComponent();
		if (ap != nullptr)
		{
			ap->SetPoints(data.max_action_points);
		}

		// Apply spell slots from CharacterData
		SpellSlots* spell_slots = dragon->GetSpellSlots();
		if (spell_slots != nullptr && !data.spell_slots.empty())
		{
			dragon->SetSpellSlots(data.spell_slots);
		}

		Engine::GetLogger().LogDebug(
			"CharacterFactory: Created Dragon from JSON at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ") - HP: " + std::to_string(data.max_hp) +
			", Speed: " + std::to_string(data.speed));
	}
	else
	{
		Engine::GetLogger().LogError("CharacterFactory: DataRegistry not found, using default Dragon stats");
	}

	return dragon;
}

// Developer D: Fighter-specific creation with JSON data loading
std::unique_ptr<Fighter> CharacterFactory::CreateFighter(Math::ivec2 position)
{
	std::unique_ptr<Fighter> fighter = std::make_unique<Fighter>(position);

	// Developer D: Load CharacterData from DataRegistry JSON
	DataRegistry* registry = Engine::GetGameStateManager().GetGSComponent<DataRegistry>();
	if (registry != nullptr)
	{
		CharacterData data = registry->GetCharacterData("Fighter");

		// Developer D: Convert CharacterData (JSON) to CharacterStats (game)
		CharacterStats stats = ConvertToCharacterStats(data);

		// Apply stats to StatsComponent
		StatsComponent* stats_comp = fighter->GetStatsComponent();
		if (stats_comp != nullptr)
		{
			*stats_comp = StatsComponent(stats);
		}

		// Apply action points from CharacterData
		ActionPoints* ap = fighter->GetActionPointsComponent();
		if (ap != nullptr)
		{
			ap->SetPoints(data.max_action_points);
		}

		// Apply spell slots from CharacterData
		SpellSlots* spell_slots = fighter->GetSpellSlots();
		if (spell_slots != nullptr && !data.spell_slots.empty())
		{
			fighter->SetSpellSlots(data.spell_slots);
		}

		Engine::GetLogger().LogDebug(
			"CharacterFactory: Created Fighter from JSON at (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ") - HP: " + std::to_string(data.max_hp) +
			", Speed: " + std::to_string(data.speed));
	}
	else
	{
		Engine::GetLogger().LogError("CharacterFactory: DataRegistry not found, using default Fighter stats");
	}

	return fighter;
}