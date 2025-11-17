/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GamePlay.h
Project:    CS230 Engine
Author:     Seungju Song
Created:    November 5, 2025
*/
#pragma once
#include "../../../CS200/RGBA.hpp"
#include "../../../Engine/Engine.hpp"
#include "../../../Engine/Fonts.h"
#include "../../../Engine/GameState.hpp"
#include "../../../Engine/Texture.hpp"
#include "../StateComponents/GridSystem.h"
#include "../Types/Events.h"

class Dragon;
class Fighter;

class GamePlay : public CS230::GameState
{
public:
	GamePlay();
	void		  Load() override;
	void		  Update(double dt) override;
	void		  Unload() override;
	void		  Draw() override;
	void		  DrawImGui() override;
	gsl::czstring GetName() const override;

private:
	enum class PlayerActionState
	{
		None,
		SelectingMove,
		SelectingAction,
		TargetingForAttack,
		TargetingForSpell
	};
	PlayerActionState currentPlayerState = PlayerActionState::None;

	struct DamageText
	{
		std::string text;
		Math::vec2	position = { 0, 0 };
		Math::vec2	size	 = { 0, 0 };
		double		lifetime;
	};

	std::vector<DamageText> damage_texts;

	void OnCharacterDamaged(const CharacterDamagedEvent& event);

	Fighter* fighter;
	Dragon*	 dragon;
	bool	 game_end;

	std::vector<Math::ivec2> CalculateSimplePath(Math::ivec2 start, Math::ivec2 end);
};