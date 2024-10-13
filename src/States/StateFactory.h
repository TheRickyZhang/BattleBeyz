#pragma once

#include <memory>
#include "GameState.h"
#include "StateIdentifiers.h"

// Forward declaration of state classes
class HomeState;
class ActiveState;
class CustomizeState;
class AboutState;

class StateFactory {
public:
    // Static method to create a state based on the GameStateType
    static std::unique_ptr<GameState> createState(GameStateType stateType);
};
