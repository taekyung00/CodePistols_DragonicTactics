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


#include "Engine/Engine.hpp"
#include "Engine/GameObject.h"
#include "Engine/GameState.hpp"
#include "Engine/Component.h"
#include "Engine/ComponentManager.h"
#include "Engine/GameObjectManager.h"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Sprite.h"
#include "Engine/Animation.h"


#include "Engine/Vec2.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Rect.hpp"


#include "Game/DragonicTactics/Types/GameTypes.h"
#include "Game/DragonicTactics/Types/CharacterTypes.h"
#include "Game/DragonicTactics/Types/GameObjectTypes.h"
#include "Game/DragonicTactics/Types/Events.h"


#include <SDL.h>
#include <imgui.h>


#include <gsl/gsl>