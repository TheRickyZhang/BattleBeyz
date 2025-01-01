#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>
#include <functional>
#include <glm/glm.hpp>

#include <imgui.h>

#include "StateIdentifiers.h"

#define MINI_CASE_SENSITIVE
#include <mini/ini.h>

class GameState;
class ShaderProgram;
class PhysicsWorld;
class Camera;
class QuadRenderer;
class TextRenderer;
class TextureManager;
class ProfileManager;
class MessageLog;
class InputManager;
class Timer;

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool init(const char* title, int width, int height);
    void initTimers();
    void cleanup();

    void changeState(GameStateType stateType);
    GameState* getGameState();  // Return current state.
    void pushState(GameStateType stateType);
    void popState();

    void handleEvents();
    void update();
    void draw();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    bool running() const {
        return isRunning && !glfwWindowShouldClose(window);
    };
    void quit() {
        isRunning = false;
    };
    GLFWwindow* getWindow() const {
        return window;
    };

    // Everything default initialized {} is initialized in the constructor

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

    Camera* camera = nullptr;

    ShaderProgram* objectShader{};
    ShaderProgram* backgroundShader{};

    TextRenderer* textRenderer{};
    QuadRenderer* quadRenderer{};

    float prevTime{};
    float deltaTime{};

    // Singleton Managers
    TextureManager& tm;     // Textures
    ProfileManager& pm;     // Profiles (and corresponding data)
    InputManager& im;       // Key/Mouse Input
    MessageLog& ml;         // Message Logging

    // INI handling
    mINI::INIFile* iniFile{};
    mINI::INIStructure* iniData{};

    // Other various global states
    bool boundCamera{};
    bool debugMode{};
    bool paused{};

    std::vector<Timer> timers;  // TODO: In very future, if have dynamic timers separate from the fixed timers (frame rate, etc)
    std::vector<std::function<void()>> timerCallbacks;

    std::string fpsText{};
    std::string coordsText{};

private:
    std::unique_ptr<GameState> createState(GameStateType stateType);
    std::vector<std::unique_ptr<GameState>> stateStack;
    GLFWwindow* window;
    bool isRunning;
    bool debugScreenActive = false;

    void handleGlobalEvents();
};
