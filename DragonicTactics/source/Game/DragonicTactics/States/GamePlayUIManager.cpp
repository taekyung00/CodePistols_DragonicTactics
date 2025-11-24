/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlayUIManager.cpp
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 24, 2025
*/
#include "GamePlay.h"
#include "./CS200/IRenderer2D.hpp"
#include "./CS200/NDC.hpp"
#include "./Engine/Engine.hpp"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.hpp"
#include "./Engine/Logger.hpp"
#include "./Engine/TextManager.hpp"
#include "./Engine/Window.hpp"

#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/Singletons/CombatSystem.h"
#include "Game/DragonicTactics/Singletons/DiceManager.h"
#include "Game/DragonicTactics/Singletons/EventBus.h"
#include "Game/MainMenu.h"

#include "Engine/Input.hpp"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/Singletons/SpellSystem.h"
#include <imgui.h>
#include "GamePlayUIManager.h"

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size) {
    DamageText text{
        std::to_string(damage),
        position,
        size,
        1.0  
    };
    m_damage_texts.push_back(text);
}

void GamePlayUIManager::Update(double dt) {

    for (auto& text : m_damage_texts) {
        text.lifetime -= dt;
    }

    m_damage_texts.erase(
        std::remove_if(m_damage_texts.begin(), m_damage_texts.end(),
            [](const DamageText& text) { return text.lifetime <= 0; }),
        m_damage_texts.end()
    );
}

void GamePlayUIManager::Draw(Math::TransformationMatrix camera_matrix) {
    for (const auto& text : m_damage_texts) {
        Engine::GetTextManager().Draw(text.text, text.position, text.size);
    }
}
