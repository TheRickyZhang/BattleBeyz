#pragma once

#include <memory>
#include "GameState.h"

// TODO: Forward declaration of state classes
class HomeState;
class ActiveState;
class CustomizeState;
class AboutState;

class StateFactory {
public:
    // Static method to create a state based on the GameStateType
    static std::unique_ptr<GameState> createState(GameEngine* game, GameStateType stateType);
};
