/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  CharacterStates.cpp
Project:    GAM200
Author:     Seungju Song
Created:    Oct 08, 2025
*/

#include "CharacterStates.h"
#include "Character.h" // 또는 GameObject.h - 실제 클래스 정보가 필요하므로 include

// State_Idle의 함수들을 실제로 구현
void State_Idle::Enter([[maybe_unused]] CS230::GameObject* object) {
    // Idle 상태에 들어왔을 때의 로직을 여기에 작성...
}
void State_Idle::Update([[maybe_unused]] CS230::GameObject* object, [[maybe_unused]] double dt) {
    // Idle 상태일 때 매 프레임 실행될 로직...
    // Character* character = static_cast<Character*>(object);
    // character->DecideAction(); // 예를 들어 이런 식으로 AI 두뇌를 호출
}
void State_Idle::CheckExit([[maybe_unused]] CS230::GameObject* object) {
    // Idle 상태를 벗어날 조건을 여기에 작성...
}

// ... State_Moving, State_Attacking에 대한 구현 ...

// State_TurnEnd의 함수들을 실제로 구현
void State_TurnEnd::Enter([[maybe_unused]] CS230::GameObject* object) {
    // 턴 종료 상태에 들어왔을 때의 로직...
}
void State_TurnEnd::Update([[maybe_unused]] CS230::GameObject* object, [[maybe_unused]] double dt) {
    // 턴 종료 애니메이션 등을 여기서 처리...
}
void State_TurnEnd::CheckExit([[maybe_unused]] CS230::GameObject* object) {
    // 다음 턴으로 넘어가는 등의 조건...
}