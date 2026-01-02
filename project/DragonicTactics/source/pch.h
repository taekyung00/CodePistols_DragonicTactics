/**
 * \file
 * \author Taekyung Ho
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "Engine/Engine.h"
#include "Engine/GameObject.h"
#include "Engine/GameState.h"
#include "Engine/Component.h"
#include "Engine/ComponentManager.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.h"
#include "Engine/Logger.h"
#include "Engine/Input.h"
#include "Engine/Window.h"
#include "Engine/TextureManager.h"
#include "Engine/Sprite.h"
#include "Engine/Animation.h"

#include "Engine/Vec2.h"
#include "Engine/Matrix.h"
#include "Engine/Rect.h"

#include "Game/DragonicTactics/Types/GameTypes.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameObjectTypes.h"
#include "Game/DragonicTactics/Types/Events.h"

#include <SDL.h>
#include <imgui.h>

#include <gsl/gsl>

// Cross-platform function name macro
#if defined(__GNUC__) || defined(__clang__)
#  define FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define FUNC_NAME __FUNCSIG__
#else
#  define FUNC_NAME __func__
#endif