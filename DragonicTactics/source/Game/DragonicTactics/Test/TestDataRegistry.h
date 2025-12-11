/**
 * \file
 * \author Ginam Park
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

// ===== Basic JSON Loading Tests =====
bool TestDataRegistry_LoadJSON();
bool TestDataRegistry_GetValue();
bool TestDataRegistry_HasKey();
bool TestDataRegistry_GetArray();

// ===== Character Data Tests =====
bool TestDataRegistry_GetCharacterData();
bool TestDataRegistry_DragonStats();
bool TestDataRegistry_FighterStats();

// ===== Hot-Reload Tests =====
bool TestDataRegistry_ReloadAll();
bool TestDataRegistry_FileModificationDetection();

// ===== Validation Tests =====
bool TestDataRegistry_ValidateCharacterJSON();
bool TestDataRegistry_InvalidJSONHandling();

extern bool TestDataRegistry;
