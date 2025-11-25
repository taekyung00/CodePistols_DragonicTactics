#include "TestSpellSystem.h"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Game/DragonicTactics/Objects/Dragon.h"
#include "Game/DragonicTactics/StateComponents/SpellSystem.h"
#include "Game/DragonicTactics/StateComponents/EventBus.h"
#include "Game/DragonicTactics/StateComponents/GridSystem.h"
#include "Game/DragonicTactics/Test/TestAssert.h"

bool TestSpellRegistration()
{
	SpellSystem spellSys;

	// Test: Register a spell
	MockFireball* fireball = new MockFireball();
	spellSys.RegisterSpell("Fireball", fireball);

	// Test: Retrieve the spell
	MockSpellBase* retrieved = spellSys.GetSpell("Fireball");
	ASSERT_TRUE(retrieved != nullptr);
	ASSERT_TRUE(retrieved->GetName() == "Fireball");

	return true;
}

bool TestSpellCasting()
{
	GridSystem grid;
	EventBus eventBus;

	SpellSystem spellSys;
	spellSys.SetEventBus(&eventBus);

	// Register Fireball
	spellSys.RegisterSpell("Fireball", new MockFireball());

	// Create caster with spell slots
	Character* caster = new Dragon(Math::vec2{ 0, 0 });
	caster->SetSpellSlots(
		{
			{ 3, 2 }
	 }); // 3 level 2 slots

	// Create target
	Character* target = new Dragon(Math::vec2{ 3, 3 });
	grid.AddCharacter(target, Math::vec2{ 3, 3 });

	// Test: Cast spell through SpellSystem
	MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{ 3, 3 });

	ASSERT_TRUE(result.success);
	ASSERT_TRUE(result.total_damage > 0);
	std::cout << caster->GetSpellSlotCount(3) << " should be 1" << std::endl;
	ASSERT_TRUE(caster->GetSpellSlotCount(3) == 1); // Used 1 slot  ///////fail

	delete caster;
	delete target;
	grid.Reset();
	return true;
}

bool TestSpellUpcast()
{
	Character* caster = new Dragon(Math::vec2{ 0, 0 });

	caster->SetSpellSlots(
		{
			{ 1, 2 }
	 }); // 1 level 2 slot
	caster->SetSpellSlots(
		{
			{ 2, 3 }
	 }); // 2 level 3 slots

	EventBus eventBus;

	SpellSystem spellSys;
	spellSys.SetEventBus(&eventBus);
	spellSys.RegisterSpell("Fireball", new MockFireball());

	// Test: Upcast Fireball to level 3
	MockSpellResult result = spellSys.CastSpell(caster, "Fireball", Math::vec2{ 3, 3 }, 3);

	ASSERT_TRUE(result.success);
	std::cout << caster->GetSpellSlotCount(1) << " should be 2" << std::endl;
	ASSERT_TRUE(caster->GetSpellSlotCount(1) == 2); // Didn't use level 2 slot
	std::cout << caster->GetSpellSlotCount(2) << " should be 3" << std::endl;
	ASSERT_TRUE(caster->GetSpellSlotCount(2) == 3); // Used level 3 slot

	delete caster;
	return true;
}

bool TestGetAvailableSpells()
{
	Character* caster = new Dragon(Math::vec2{ 0, 0 });
	caster->SetSpellSlots(
		{
			{ 2, 1 }
	 });
	caster->SetSpellSlots(
		{
			{ 3, 2 }
	 });

	SpellSystem spellSys;
	spellSys.RegisterSpell("Fireball", new MockFireball());		// Level 2
	spellSys.RegisterSpell("CreateWall", new MockCreateWall()); // Level 1

	// Test: Get available spells
	std::vector<std::string> available = spellSys.GetAvailableSpells(caster);

	ASSERT_TRUE(available.size() == 2);
	ASSERT_TRUE(std::find(available.begin(), available.end(), "Fireball") != available.end());
	ASSERT_TRUE(std::find(available.begin(), available.end(), "CreateWall") != available.end());

	delete caster;
	return true;
}

bool TestPreviewSpellArea()
{
	SpellSystem spellSys;
	spellSys.RegisterSpell("Fireball", new MockFireball());

	// Test: Preview Fireball area
	std::vector<Math::vec2> tiles = spellSys.PreviewSpellArea("Fireball", Math::vec2{ 4, 4 });

	ASSERT_TRUE(tiles.size() > 0);
	// Fireball has radius 2, so should affect multiple tiles
	ASSERT_TRUE(tiles.size() >= 9); // At least 3x3 area

	return true;
}

void RunSpellSystemTests()
{
	std::cout << "\n=== SpellSystem Tests ===\n";
	std::cout << (TestSpellRegistration() ? "O" : "X") << " Spell Registration\n";
	std::cout << (TestSpellCasting() ? "O" : "X") << " Spell Casting\n";
	std::cout << (TestSpellUpcast() ? "O" : "X") << " Spell Upcasting\n";
	std::cout << (TestGetAvailableSpells() ? "O" : "X") << " Get Available Spells\n";
	std::cout << (TestPreviewSpellArea() ? "O" : "X") << " Preview Spell Area\n";
}
