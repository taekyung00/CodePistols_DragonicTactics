/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GameStateManager.hpp"
#include "GameObjectManager.h"

namespace CS230
{
    void GameStateManager::PopState()
    {
        using namespace std::literals;
        auto* const state = mGameStateStack.back().get();
        mToClear.push_back(std::move(mGameStateStack.back()));
        mGameStateStack.erase(mGameStateStack.end() - 1);
        Engine::GetLogger().LogEvent("Exiting state "s + state->GetName());
        state->Unload();
    }

    void GameStateManager::Update(double dt)
    {
        mToClear.clear();
        mGameStateStack.back()->Update(dt);
        if (!mGameStateStack.empty())
        {
            GameObjectManager* current_gameobject_manager = mGameStateStack.back()->GetGSComponent<GameObjectManager>();
            if (current_gameobject_manager != nullptr)
            {
                current_gameobject_manager->CollisionTest();
            }
        }
    }

    void GameStateManager::Draw()
    {
        for (auto& game_state : mGameStateStack)
        {
            game_state->Draw();
        }
    }

    void GameStateManager::DrawImGui()
    {
        if (!mGameStateStack.empty())
        {
            mGameStateStack.back()->DrawImGui();
        }
    }

    void GameStateManager::Clear()
    {
        while (!mGameStateStack.empty())
            PopState();
        mToClear.clear();
    }

}
