#pragma once

#include <memory>
#include <iostream>
#include <functional>
#include <queue>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include "StateIdentifiers.h"
#include "Timer.h"

#define MINI_CASE_SENSITIVE
#include <mini/ini.h>

class GameState;
class ObjectShader;
class BackgroundShader;
class PhysicsWorld;
class Camera;
class MessageLog;

class FontManager;
class InputManager;
class ProfileManager;
class TextureManager;
class QuadRenderer;
class TextRenderer;

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    bool init(const char* title, int width, int height);

    GameState* getGameState();
    void changeState(std::unique_ptr<GameState> newState);
    void pushState(std::unique_ptr<GameState> state);
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

    // Singleton Managers   
    FontManager& fm;        // ImGui fonts
    InputManager& im;       // Key/Mouse Input
    MessageLog& ml;         // Message Logging
    ProfileManager& pm;     // Profiles (and corresponding data)
    TextureManager& tm;     // Textures
    ImGuiIO& io;

    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 projection{};
    glm::mat4 backgroundModel{};
    glm::mat4 backgroundView{};
    glm::mat4 orthoProjection{};

    PhysicsWorld* physicsWorld{};

    Camera* camera = nullptr;

    ObjectShader* objectShader{};
    BackgroundShader* backgroundShader{};

    TextRenderer* textRenderer{};
    QuadRenderer* quadRenderer{};

    float currTime{};
    float prevTime{};
    float deltaTime{};

    // Other various global states
    bool boundCamera{};
    bool debugMode{};
    bool paused{};

    std::priority_queue<Timer, std::vector<Timer>, TimerComparison> timers;

    std::string fpsText{};
    std::string coordsText{};

private:
    void cleanup();
    std::vector<std::unique_ptr<GameState>> stateStack;

    GLFWwindow* window;
    bool isRunning;
    bool debugScreenActive = false;

    // Used in execution
    void handleGlobalEvents();
    void updateTimers(float currentTime);
    void drawDebugScreen();

    // Level 1 helper functions
    bool initializeGLFW();
    bool createWindow(const char* title, int width, int height);
    bool initializeGLEW();
    void setupImGuiAndCallbacks();
    bool attemptSaveDataLoad();
    void initCamera();
    void initShaders();
    void initRenderers();
    void initTimers();

    // Level 2 helper functions
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    void onResize(int width, int height);
};
