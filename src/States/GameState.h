#pragma once

#include <string>
#include <unordered_map>

#include <imgui.h>

// Used by every state
#include "GameEngine.h"
#include "MessageLog.h"   
#include "StateFactory.h"

class GameState {
public:
    GameState(GameEngine* _game) : game(_game) {};
    virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void cleanup() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvents() = 0;
    virtual void onResize(int width, int height) = 0;
    virtual void update(float deltaTime) = 0;

    // Draws the screen. Every implementation assumes ImGui::begin() is already called, but must end with an ImGui::End()
    virtual void draw() = 0;

    virtual GameStateType getStateType() const = 0;

    static void initStyleParams() {
        frameSpacingX = ImGui::GetStyle().FramePadding.x;
        frameSpacingY = ImGui::GetStyle().FramePadding.y;
        spacing = ImGui::GetStyle().ItemSpacing.x;
    }
public:
    GameEngine* game;

protected:
    // Common shorthands for ImGui display values. Only change from settings modification
    static float frameSpacingX;
    static float frameSpacingY;
    static float spacing;

    void renderBackground(GameEngine* game, const std::string& textureString);

    // Function to render a window with text, buttons, and optional text before/after
    void renderWindowWithButtons(GameEngine* game, const std::string& windowText,
        const std::vector<std::string>& buttonText,
        const std::string& beforeText = "", const std::string& afterText = "");

    // TOLOOK: Does not distinguish between swtiching and pushing states
    // Map button text to the state they will transition onto
    static const std::unordered_map<std::string, GameStateType> buttonStateMap;
};
