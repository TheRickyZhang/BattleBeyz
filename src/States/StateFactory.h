#pragma once

#include <memory>
#include <vector>

class Stadium;
class Beyblade;
//class PhysicsWorld;

#include "GameState.h"

namespace StateFactory {
    // Static method to create a state based on the GameStateType
    std::unique_ptr<GameState> createState(GameEngine* game, GameStateType stateType);

    //// Pass in the stadium, beyblades, and physics
    //std::unique_ptr<GameState> createActiveState(
    //    GameEngine* game,
    //    std::vector<std::shared_ptr<Stadium>> stadiums,
    //    std::vector<std::shared_ptr<Beyblade>> beyblades,
    //    std::shared_ptr<PhysicsWorld> physicsWorld
    //);

    // Specifically to create differentiable loading states
    std::unique_ptr<GameState> createLoadingState(
        GameEngine* game,
        const std::vector<std::function<bool()>>& tasks,
        std::function<void()> onComplete
    );
}
