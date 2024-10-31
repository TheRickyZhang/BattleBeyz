#pragma once

#include "GameState.h"
#include "Stadium.h"
#include "Beyblade.h"

class ActiveState : public GameState {
public:
    ActiveState(GameEngine* _game) : GameState(_game) {};

    void init() override;
    void cleanup() override;

    void pause() override;
    void resume() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void draw() override;

    GameStateType getStateType() const override { return GameStateType::ACTIVE; }

    // Keyboard and mouse event handlers

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
private:
    bool escPressed = false;

    float imguiColor[3] = { 0.45f, 0.55f, 0.60f };
    bool showInfoScreen = false; // Only one bool here

    GLuint floorVAO, floorVBO, floorEBO; // TODO: floor class? Right now assum only one floor;
    std::vector<Stadium*> stadiums;
    std::vector<Beyblade*> beyblades;

    void ActiveState::drawInfoScreen();
};
