#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "States/GameState.h"
#include <unordered_map>
#include <memory>
#include <iostream>
#include <functional>
#include "States/StateFactory.h"
#include "PhysicsWorld.h"
#include "Camera.h"
#include "QuadRenderer.h"
#include "TextRenderer.h"
#include "Callbacks.h"
#include "Timer.h"

#define MINI_CASE_SENSITIVE
#include "mini/ini.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool init(const char* title, int width, int height);
    void initTimers();
    void cleanup();

    void changeState(GameStateType stateType);
    void pushState(GameStateType stateType);
    void popState();

    void handleEvents();
    void update();
    void draw();

    bool running() const {
        return isRunning && !glfwWindowShouldClose(window);
    };
    void quit() {
        isRunning = false;
    };
    GLFWwindow* getWindow() const {
        return window;
    };

    int windowWidth{};
    int windowHeight{};
    float aspectRatio{};
    int minWidth{};
    int minHeight{};

    int nMonitors{};
    GLFWmonitor** monitors{};
    const GLFWvidmode* videoMode{};
    int monitorX{};
    int monitorY{};

    ImGuiIO& io;

    // TODO: Modify to a <string, ImFont*> map
    ImFont* defaultFont{};
    ImFont* titleFont{};
    ImFont* attackFont{};

    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 projection{};
    glm::mat4 backgroundModel{};
    glm::mat4 backgroundView{};
    glm::mat4 orthoProjection{};

    PhysicsWorld* physicsWorld{};
    Camera mainCamera{};
    CameraState* cameraState{};

    ShaderProgram* objectShader{};
    ShaderProgram* backgroundShader{};

    TextRenderer* textRenderer{};

    float prevTime{};
    float deltaTime{};

    float imguiColor[3];

    QuadRenderer* quadRenderer{};

    Texture* hexagonPattern{};
    Texture* smallHexagonPattern{};
    Texture* homeScreenTexture{};
    Texture* backgroundTexture{};
    Texture* floorTexture{};
    Texture* stadiumTexture{};

    // INI handling
    mINI::INIFile* iniFile{};
    mINI::INIStructure* iniData{};

    // Other various global states
    bool boundCamera{};
    bool debugMode{};
    int selectedBeyblade{};

    std::vector<Timer> timers;  // TODO: In very future, if have dynamic timers separate from the fixed timers (frame rate, etc)
    std::vector<std::function<void()>> timerCallbacks;

    std::string fpsText{};
    std::string coordsText{};

private:
    std::unique_ptr<GameState> createState(GameStateType stateType);

    std::vector<std::unique_ptr<GameState>> stateStack;
    GLFWwindow* window;
    bool isRunning;
};
