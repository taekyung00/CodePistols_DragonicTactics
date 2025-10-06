/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameStateManager.h
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    March 11, 2025
*/
#pragma once
#include "GameState.h"
#include <vector>

namespace CS230 {
    class GameStateManager {
    public:
        GameStateManager();

        void Update(double);
        void AddGameState(GameState& gamestate);
        void SetNextGameState(int index);
        void ClearNextGameState();
        void ReloadState();
        bool HasGameEnded();

        template<typename T>
        T* GetGSComponent() { return current_gamestate->GetGSComponent<T>(); }

    private:
        enum class Status {
            STARTING,
            LOADING,
            UPDATING,
            UNLOADING,
            STOPPING,
            EXIT
        };

        Status status;
        std::vector<GameState*> gamestates;
        GameState* current_gamestate;
        GameState* next_gamestate;
    };
}
