#pragma once

#include <string>
#include <unordered_map>

// Used by every state
#include "GameEngine.h"
#include "MessageLog.h"   

class GameState {
public:
    GameState(GameEngine* _game) : game(_game) {};
    virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void cleanup() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvents() = 0;
    virtual void update(float deltaTime) = 0;

    // Draws the screen. Every implementation assumes ImGui::begin() is already called, but must end with an ImGui::End()
    virtual void draw() = 0;

    virtual GameStateType getStateType() const = 0;

public:
    GameEngine* game;

protected:
    void renderBackground(GameEngine* game, const std::string& textureString);

    // Function to render a window with text, buttons, and optional text before/after
    void renderWindowWithButtons(GameEngine* game, const std::string& windowText,
        const std::vector<std::string>& buttonText,
        const std::string& beforeText = "", const std::string& afterText = "");

    // Map button text to the state they will transition onto (TODO: Does not distinguish between swtiching and pushing states)
    std::unordered_map<std::string, GameStateType> buttonStateMap = {
        // Main Screens
        { "Back", GameStateType::HOME }, { "Home", GameStateType::HOME },
        { "Start Game", GameStateType::ACTIVE }, { "Resume Game", GameStateType::ACTIVE},
        { "Profiles & Beyblades", GameStateType::CUSTOMIZE },
        { "Settings", GameStateType::SETTINGS},
        { "About", GameStateType::ABOUT },

        // Secondary Screens
        { "Loading", GameStateType::LOADING },
        { "Pause", GameStateType::PAUSE },
    };

};
