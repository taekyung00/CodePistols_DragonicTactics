/**
 * \file
 * \date 2026 Spring
 * \copyright DigiPen Institute of Technology
 */
#include "pch.h"

#include "Rogue.h"
#include "Engine/Engine.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Game/DragonicTactics/Objects/Actions/ActionAttack.h"
#include "Game/DragonicTactics/Objects/Components/ActionPoints.h"
#include "Game/DragonicTactics/Objects/Components/StatsComponent.h"

Rogue::Rogue(Math::ivec2 start_coordinates)
  : Character(
      CharacterTypes::Rogue, start_coordinates, 1,
      {
        { 1, 2 },
        { 2, 2 }
      })
{
  // 스탯은 CharacterFactory::CreateRogue가 DataRegistry로 초기화
  m_action_list.push_back(new ActionAttack());
  AddGOComponent(new CS230::Sprite("Assets/sprites/rogue.spt", this));
}

// void Rogue::Draw(Math::TransformationMatrix camera_matrix , unsigned int color, float depth)
// {
// #ifdef _DEBUG
//     // 1. ShakeComponent에서 현재 프레임의 흔들림 오프셋 가져오기
//     Math::vec2 shakeOffset = GetShakeComponent()->GetOffset();

//     // 2. 흔들림 오프셋이 0이 아니라면 (진동 중이라면)
//     if (shakeOffset.x != 0.0 || shakeOffset.y != 0.0)
//     {
//         // 현재 캐릭터의 진짜 위치 임시 저장
//         Math::vec2 originalPos = GetPosition(); 

//         // 렌더링을 위해 위치에 오프셋 더하기 (타입 오류 방지를 위해 double로 캐스팅)
//         SetPosition({ 
//             originalPos.x + static_cast<double>(shakeOffset.x), 
//             originalPos.y + static_cast<double>(shakeOffset.y) 
//         });

//         // 흔들린 위치로 부모의 Draw 호출 (실제 화면에 그리기)
//         CS230::GameObject::Draw(camera_matrix, color, depth);

//         // 매우 중요. 화면에 그리고 난 후, 캐릭터의 위치를 다시 원래대로 원상복구!
//         SetPosition(originalPos);
//     }
//     else
//     {
//         // 흔들림이 없을 때는 그냥 평소대로 그리기
//         CS230::GameObject::Draw(camera_matrix, color, depth);
//     }
// #endif
// }

void Rogue::OnTurnStart()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " - BEGIN");
  Character::OnTurnStart();
}

void Rogue::Action()
{
  if (IsAIControlled())
  {
    AISystem*  ai       = Engine::GetGameStateManager().GetGSComponent<AISystem>();
    AIDecision decision = ai->MakeDecision(this);
    ai->ExecuteDecision(this, decision);
  }
}

bool Rogue::IsAIControlled() const
{
  return true;
}

void Rogue::OnTurnEnd()
{
  Engine::GetLogger().LogDebug(std::string(FUNC_NAME) + " called");
  Character::OnTurnEnd();
}

void Rogue::Update(double dt)
{
  Character::Update(dt);
}

void Rogue::TakeDamage(int damage, Character* attacker)
{
  Character::TakeDamage(damage, attacker);
}
