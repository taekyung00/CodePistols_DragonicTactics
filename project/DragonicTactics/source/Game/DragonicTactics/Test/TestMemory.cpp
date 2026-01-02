/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "TestMemory.h"
#include "Game/DragonicTactics/Factories/CharacterFactory.h"
#include "Game/DragonicTactics/Objects/Character.h"
#include "TestAssert.h"
#include "pch.h"
#include <memory>

void TestOwnershipTransfer()
{
  auto go_manager = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
  auto factory	  = Engine::GetGameStateManager().GetGSComponent<CharacterFactory>();

  auto		 character = factory->Create(CharacterTypes::Dragon, { 0, 0 });
  Character* raw_ptr   = character.get();

  go_manager->Add(std::move(character));

  ASSERT_TRUE(character == nullptr);

  ASSERT_TRUE(raw_ptr != nullptr);

  auto objects = go_manager->GetAllRaw();
  ASSERT_TRUE(objects.size() == 1);
  ASSERT_TRUE(objects[0] == raw_ptr);
}

void TestUnloadNoLeak()
{
  auto go_manager = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
  for (int i = 0; i < 5; ++i)
  {
	auto character = CharacterFactory::Create(CharacterTypes::Fighter, { i, 0 });
	go_manager->Add(std::move(character));
  }

  ASSERT_TRUE(go_manager->GetAllRaw().size() == 6); // counting the one from previous test

  go_manager->Unload();

  ASSERT_TRUE(go_manager->GetAllRaw().size() == 0);
}