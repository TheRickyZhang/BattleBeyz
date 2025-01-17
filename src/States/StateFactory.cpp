#include "StateFactory.h"
#include "StateIdentifiers.h"

#include "AboutState.h"
#include "ActiveState.h"
#include "CustomizeState.h"
#include "LoadingState.h"
#include "HomeState.h"
#include "PauseState.h"
#include "SelectionState.h"
#include "SettingsState.h"

#include "Stadium.h"
#include "Beyblade.h"
#include "PhysicsWorld.h"

using namespace std;

namespace StateFactory {
    // Implement the createState function that returns a unique_ptr to the correct GameState based on GameStateType
    unique_ptr<GameState> createState(GameEngine* game, GameStateType stateType) {
        switch (stateType) {
            case GameStateType::ABOUT: return make_unique<AboutState>(game); 
            case GameStateType::CUSTOMIZE: return make_unique<CustomizeState>(game); 
            case GameStateType::HOME: return make_unique<HomeState>(game);
            case GameStateType::PAUSE: return make_unique<PauseState>(game); 
            case GameStateType::SELECTION: return make_unique<SelectionState>(game);
            case GameStateType::SETTINGS: return make_unique<SettingsState>(game);
            default: {
                cout << "Unknown game state passed to state factory" << endl;
                return nullptr;
            }
        }
    }


    //unique_ptr<GameState> createActiveState(
    //    GameEngine* game,
    //    vector<shared_ptr<Stadium>> stadiums,
    //    vector<shared_ptr<Beyblade>> beyblades,
    //    shared_ptr<PhysicsWorld> physicsWorld
    //) {
    //    return make_unique<ActiveState>(
    //        game,
    //        move(stadiums),   // Transfer ownership of stadiums
    //        beyblades,             // Pass non-owning raw pointers for beyblades
    //        move(physicsWorld) // Transfer ownership of the physics world
    //    );
    //}

    // Overload that specifically creates a LoadingState
    unique_ptr<GameState> createLoadingState(
        GameEngine* game,
        const vector<function<bool()>>& tasks,
        function<void()> onComplete
    ) {
        return make_unique<LoadingState>(game, tasks, onComplete);
    }
}
