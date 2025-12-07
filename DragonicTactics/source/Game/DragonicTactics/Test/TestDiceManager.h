#pragma once

// ===== Basic Dice Rolling Tests =====
bool TestDiceManager_RollDice();
bool TestDiceManager_RollMultipleDice();
bool TestDiceManager_RollFromString();

// ===== Dice String Parsing Tests =====
bool TestDiceManager_ParseSimpleDice();
bool TestDiceManager_ParseDiceWithModifier();
bool TestDiceManager_ParseInvalidString();

// ===== Seed and Determinism Tests =====
bool TestDiceManager_SetSeed();
bool TestDiceManager_DeterministicRolls();
bool TestDiceManager_DifferentSeeds();

// ===== Range Tests =====
bool TestDiceManager_RollRange();
bool TestDiceManager_MaxRoll();
bool TestDiceManager_MinRoll();

extern bool TestDiceManager;
