#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "BeybladeBody.h"
#include "StateIdentifiers.h"


class GameEngine;

class GameState {
public:
    virtual ~GameState() = default;

    virtual void init() = 0;
    virtual void cleanup() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual void handleEvents(GameEngine* game) = 0;
    virtual void update(GameEngine* game, float deltaTime) = 0;

    // Draws the screen. Every implementation assumes ImGui::begin() is already called, but must end with an ImGui::End()
    virtual void draw(GameEngine* game) = 0;

    virtual GameStateType getStateType() const = 0;
protected:

    void renderBackground(GameEngine* game, const std::string& textureString);

    // Function to render a window with text, buttons, and optional text before/after
    void renderWindowWithButtons(GameEngine* game, const std::string& windowText,
        const std::vector<std::string>& buttonText,
        const std::string& beforeText = "", const std::string& afterText = "");

    // Map button text to the state they will transition onto (TODO: Does not distinguish between swtiching and pushing states)
    std::unordered_map<std::string, GameStateType> buttonStateMap = {
        { "Start Game", GameStateType::ACTIVE },
        { "Resume Game", GameStateType::ACTIVE},
        { "Customize Beyblades", GameStateType::CUSTOMIZE },
        { "About", GameStateType::ABOUT },
        { "Back", GameStateType::HOME },
        { "Pause", GameStateType::PAUSE },
        { "Loading", GameStateType::LOADING }
    };

};
