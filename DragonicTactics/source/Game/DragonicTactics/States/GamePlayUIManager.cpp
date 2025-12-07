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
#include "./CS200/IRenderer2D.h"
#include "./CS200/NDC.h"
#include "./Engine/Engine.h"
#include "./Engine/GameObjectManager.h"
#include "./Engine/GameStateManager.h"
#include "./Engine/Logger.h"
#include "./Engine/TextManager.h"
#include "./Engine/Window.h"

#include "../Debugger/DebugManager.h"
#include "../StateComponents/GridSystem.h"
#include "../StateComponents/TurnManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/MovementComponent.h"
#include "Game/DragonicTactics/Objects/Components/SpellSlots.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"
#include "Game/DragonicTactics/StateComponents/CombatSystem.h"
#include "Game/DragonicTactics/StateComponents/DiceManager.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/MainMenu.h"

#include "Engine/Input.h"
#include "Game/DragonicTactics/Objects/Components/GridPosition.h"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/Objects/Fighter.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include <imgui.h>
#include "GamePlayUIManager.h"

void GamePlayUIManager::ShowDamageText(int damage, Math::vec2 position, Math::vec2 size) {
    DamageText text{
        std::to_string(damage),
        position,
        size,
        0.5  
    };
    m_damage_texts.push_back(text);
}

void GamePlayUIManager::ShowDamageLog(std::string str, Math::vec2 position, Math::vec2 size) {
    DamageText text{
        str,
        position,
        size,
        1.0
    };
    m_damage_log.push_back(text);
}

void GamePlayUIManager::ShowGameEnd(std::string&& text){
    game_end_text = std::make_unique<std::string>(text);
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

    m_damage_log.erase(
        std::remove_if(m_damage_log.begin(), m_damage_log.end(),
            [](const DamageText& text) { return text.lifetime <= 0; }),
        m_damage_log.end()
    );
}

void GamePlayUIManager::Draw([[maybe_unused]]Math::TransformationMatrix camera_matrix) {
    auto& textMng = Engine::GetTextManager();
    for (const auto& text : m_damage_texts) {
        textMng.DrawText(text.text, text.position,Fonts::Outlined, text.size, CS200::VIOLET);
    }

    int i = 0;
    for (const auto& text : m_damage_log) {
        textMng.DrawText(text.text, text.position + Math::vec2{0, text.size.y * i},Fonts::Outlined, text.size, CS200::RED);
        ++i;
    }

    if(game_end_text) {
        auto size = Engine::GetWindow().GetSize();
        textMng.DrawText(*game_end_text, Math::ivec2{size.x / 3, size.y / 2}, Fonts::Outlined, Math::vec2{GAME_END_TEXT_SIZE, GAME_END_TEXT_SIZE}, CS200::WHITE);
    }
}
