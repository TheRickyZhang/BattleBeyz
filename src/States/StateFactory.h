#pragma once

#include <memory>
#include "GameState.h"

class StateFactory {
public:
    // Static method to create a state based on the GameStateType
    static std::unique_ptr<GameState> createState(GameEngine* game, GameStateType stateType);

    // Specifically to create differentiable loading states
    static std::unique_ptr<GameState> StateFactory::createLoadingState(
        GameEngine* game,
        const std::vector<std::function<bool()>>& tasks,
        std::function<void()> onComplete
    );
};
