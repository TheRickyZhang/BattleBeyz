#include <iostream>
#include <exception>

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "GameEngine.h"

#include "BeybladeConstants.h"
#include "Camera.h"
#include "FontManager.h"
#include "ImGuiUtils.h"
#include "InputManager.h"
#include "Utils.h"
#include "UI.h"
#include "MessageLog.h"
#include "PhysicsWorld.h"
#include "ProfileManager.h"
#include "QuadRenderer.h"
#include "ShaderPath.h"
#include "ObjectShader.h"
#include "BackgroundShader.h"
#include "StateFactory.h"
#include "TextRenderer.h"
#include "TextureManager.h"
#include "Timer.h"

using namespace std;
using namespace glm;

// Constructor with default values
GameEngine::GameEngine()
    : window(nullptr),
    isRunning(false),
    // Need to initialize these as references from constructor, as opposed to init()
    io(ImGui::GetIO()),
    tm(TextureManager::getInstance()),
    pm(ProfileManager::getInstance()),
    ml(MessageLog::getInstance()),
    im(InputManager::getInstance()),
    fm(FontManager::getInstance())
{
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
}


GameEngine::~GameEngine() {
    cleanup();
}


bool GameEngine::init(const char* title, int width, int height) {
    // Handle window sizes
    windowWidth = width;
    windowHeight = height;
    aspectRatio = static_cast<float>(width) / height;
    minWidth = width / 4;
    minHeight = height / 4;

    /*
    * Initialize All OpenGL (GLFW, GLEW)
    */
    
    // GLFW must come first
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        isRunning = false;
        return false;
    }

    // Set window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create window" << endl;
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }

    // Set the OpenGL context for this window
    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, static_cast<int>(aspectRatio * 100), 100);

    // Center window on the primary monitor
    monitors = glfwGetMonitors(&nMonitors);
    videoMode = glfwGetVideoMode(monitors[0]);
    glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);
    glfwSetWindowPos(window,
        monitorX + static_cast<int>(videoMode->width - windowWidth) / 2,
        monitorY + static_cast<int>(videoMode->height - windowHeight) / 2);
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeLimits(window, static_cast<int>(minWidth), static_cast<int>(minHeight), GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    while (glGetError() != GL_NO_ERROR) {}  // Do not remove; flushes error buffer
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
    * Initialize ImGui and attempt to import existing game data through JSONs
    */

    // Catch any errors from file importing, returning a blue screen of informational death if failing
    try {
        // TODO: Add default profiles instead of crashing if json not parsed correctly
        setupImGui(window);

        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);

        pm.loadProfilesFromFile(PROFILE_SAVE_PATH);
    }
    catch (const nlohmann::json::exception& e) {
        
        //pm.addDefaultProfiles();
        
        // Add critical error messages to the message log
        ml.addMessage("Critical error during initialization: " + string(e.what()), MessageType::ERROR);
        ml.addMessage("Please investigate the issue, and reach out to the contact email for further support");
        ml.open();

        // Enter error handling loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ml.render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }
    }
    catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "An unknown error occurred during initialization." << endl;
    }
    GameState::initStyleParams();


    /*
    * Initialize everything physics and space related
    */
    physicsWorld = new PhysicsWorld();

    // Camera and initial view initialization
    vec3 initialCameraPos(2.0f, 1.5f, 0.0f);
    vec3 lookAtPoint(0.0f, 0.0f, 0.0f);

    model = mat4(1.0f);
    view = lookAt(initialCameraPos, lookAtPoint, vec3(0.0f, 1.0f, 0.0f));
    projection = perspective(radians(45.0f), float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
    backgroundModel = mat4(1.0f);
    backgroundView = mat4(1.0f);
    orthoProjection = ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight), 0.0f, 1.0f);

    camera = new Camera(initialCameraPos, lookAtPoint, physicsWorld, windowWidth / 2.0f, windowHeight / 2.0f);


    /*
    * Set all shaders and renderers
    */
    // TODO: Give backgroundShader same treatment as objectShader, but with different functions
    backgroundShader = new BackgroundShader(BACKGROUND_VERTEX_SHADER_PATH, BACKGROUND_FRAGMENT_SHADER_PATH);
    backgroundShader->setBackgroundGlobalParams(projection, 4.0, float(glfwGetTime()));

    objectShader = new ObjectShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    objectShader->setGlobalRenderParams(view, projection, initialCameraPos);
    // TODO: Remove this? 
    objectShader->setObjectRenderParams(model, glm::vec3(1.0f));
    objectShader->setLight(LightType::Directional, vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));

    quadRenderer = new QuadRenderer(scale(mat4(1.0f), vec3(width, height, 1.0f)));


    /*
    * Initialize all managers / class - specific inits
    */

    textRenderer = new TextRenderer("./assets/fonts/OpenSans-Regular.ttf", 800, 600);
    tm.loadTexture("defaultBackground", "./assets/textures/Brickbeyz.jpg");
    tm.loadTexture("sceneBackground", "./assets/textures/HexagonSmall.jpg");
    tm.loadTexture("stadium", "./assets/textures/Hexagon.jpg");
    tm.loadTexture("floor", "./assets/textures/Wood1.jpg");
    fm.loadDefaultFonts();

    GameState::initStyleParams();

    /*
    * Other unassigned variables
    */
    boundCamera = false;
    debugMode = false;

    prevTime = 0.0f;
    deltaTime = 0.0f;

    isRunning = true;
    return true;
}

void GameEngine::initTimers() {
    // Frame rate timer - updates every second, repeats indefinitely
    timers.push_back(Timer(1.0f, true, -1.0f));
    timerCallbacks.push_back([this]() {
        stringstream ss;
        ss << fixed << setprecision(1) << "FPS: " << ImGui::GetIO().Framerate;
        fpsText = ss.str();
    });

    // Coordinate timer - updates every 0.1 seconds, repeats indefinitely
    timers.push_back(Timer(0.1f, true, -1.0f));
    timerCallbacks.push_back([this]() {
        stringstream ss;
        ss << fixed << setprecision(1) << "X: " << camera->position.x << " Y: " << camera->position.y << " Z: " << camera->position.z;
    });
}

void GameEngine::changeState(GameStateType stateType) {
    // Clear the entire stack
    while (!stateStack.empty()) {
        stateStack.back()->cleanup();
        stateStack.pop_back();
    }

    // Create and initialize the new state
    auto newState = createState(stateType);
    if (newState) {
        newState->init();
        stateStack.push_back(move(newState));
    }
}

// Hm... this loose handling of states may potentially be an issue (see: expected behavior of pushing a loadingState from init() calls)
void GameEngine::pushState(GameStateType stateType) {
    if (!stateStack.empty()) {
        stateStack.back()->pause();
    }
    auto newState = createState(stateType);
    if (newState) {
        stateStack.push_back(move(newState));
        if (!stateStack.empty()) stateStack.back()->init();
    }
}
void GameEngine::pushState(std::unique_ptr<GameState> state) {
    if (!stateStack.empty()) {
        stateStack.back()->pause();
    }
    stateStack.push_back(std::move(state));
    if (!stateStack.empty()) stateStack.back()->init();
}

void GameEngine::popState() {
    if (!stateStack.empty()) {
        stateStack.back()->cleanup();
        stateStack.pop_back();
    }

    if (!stateStack.empty()) {
        stateStack.back()->resume();
    }
    else {
        cout << "No states left in stack. Quitting." << endl;
        quit();
    }
}

unique_ptr<GameState> GameEngine::createState(GameStateType stateType) {
    return StateFactory::createState(this, stateType);
}

/**
* Get the current game state.
* 
* @return a unique_ptr to the state or nullptr if no state.
*/

GameState* GameEngine::getGameState()
{
    if (stateStack.empty()) {
        return nullptr;
    }
    else {
        return stateStack.back().get();
    }
}

void GameEngine::handleEvents() {
    // Poll GLFW events (handles window events, input events, etc.)
    glfwPollEvents();

    // Skip any updates if window is minimized
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {  
        return;
    }

    handleGlobalEvents();

    if (!stateStack.empty() && !paused) {
        stateStack.back()->handleEvents();
    }

    im.updateState();  // Must be called at end
}

void GameEngine::update() {
    if (!stateStack.empty()) {
        stateStack.back()->update(deltaTime);
    }
}

void GameEngine::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color and depth buffers

    // START IMGUI FRAME: applies to all states
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw the current state
    if (!stateStack.empty()) {
        stateStack.back()->draw();
    }

    // Display log
    ml.render();

    if (debugScreenActive) {
        // Render faded background
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(float(windowWidth), float(windowHeight)),IM_COL32(0, 0, 0, 150));
        ImGui::Begin("Debug Screen", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Window Size: %dx%d", windowWidth, windowHeight);
        double mouseX, mouseY; glfwGetCursorPos(window, &mouseX, &mouseY);
        ImGui::Text("Mouse Position: (%.1f, %.1f)", mouseX, mouseY);
        if (!stateStack.empty() && stateStack.back()->getStateType() == GameStateType::ACTIVE) {
            ImGui::Text("Coordinates: %s", coordsText.c_str());
        }
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::End();
    }

    // END IMGUI FRAME: renders to screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void GameEngine::cleanup() {
    // Save current state
    try {
        ImGui::SaveIniSettingsToDisk(IMGUI_SAVE_PATH);
        pm.saveProfilesToFile(PROFILE_SAVE_PATH);
        ml.save(MESSAGE_LOG_SAVE_DIR);
    }
    catch (const nlohmann::json::exception& e) {
        ml.addMessage("Error saving json: " + string(e.what()), MessageType::ERROR);
        ml.addMessage("Please investigate the issue, and reach out to the contact email for further support");
    }
    catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "An unknown error occurred during cleanup." << endl;
    }

    // Clean up all states in the stack
    while (!stateStack.empty()) {
        stateStack.back()->cleanup();
        stateStack.pop_back();
    }

    delete quadRenderer;
    delete objectShader;
    delete backgroundShader;
    delete textRenderer;
    delete physicsWorld;
    delete camera;

    // Shutdown ImGui properly
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy GLFW window and terminate GLFW
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();

    cout << "GameEngine cleaned up successfully." << endl;
}

void GameEngine::handleGlobalEvents() {
    if (im.keyJustPressed(GLFW_KEY_F9)) {
        ml.toggle();
    }
    if (im.keyJustPressed(GLFW_KEY_F3)) {
        debugScreenActive = !debugScreenActive;
    }
    if (im.keyJustPressed(GLFW_KEY_ESCAPE)) {
        paused = !paused;
        if (stateStack.empty()) return;
        paused ? stateStack.back()->pause() : stateStack.back()->resume();
    }

}


/**
* Various callbacks for inputManager. Important to forward to ImGui if needed!
*
* @param window                 [in] The parent window.
*
* @param key                    [in] The GLFW key code such as the letter 'A'.
*
* @param scancode               [in] The raw key number.  For example, the cursor
*                               keys do not correspond to letters.
*
* @param action                 [in] The key action that occured (press, release,
*                               or release).
*/

// Adjust the viewport when the window is resized.
void GameEngine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));

    // Optimization: return if no change in size
    if (!engine || (width == engine->windowWidth && height == engine->windowHeight)) {
        return;
    }

    // Skip updates if the window is minimized
    if (!engine || width == 0 || height == 0) {
        return;
    }

    // All logic related to resizing windows should go here. ALERT: Use this instead of recalculated window-based transformations every time!
    engine->windowWidth = width;
    engine->windowHeight = height;

    engine->textRenderer->resize(width, height); 

    glViewport(0, 0, width, height);
    engine->projection = perspective(radians(45.0f), (float)width / height, 0.1f, 100.0f);
    //engine->objectShader->use();
    //engine->objectShader->setMat4("projection", engine->projection);
}

void GameEngine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        engine->im.setKey(key, isPressed);
    }
}

void GameEngine::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) return;
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->im.setMouseButton(button, action == GLFW_PRESS);
    }
}

void GameEngine::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    if (ImGui::GetIO().WantCaptureMouse) return;
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->im.setMousePosition(xpos, ypos);
    }
}

void GameEngine::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (ImGui::GetIO().WantCaptureMouse) return;
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->im.setScrollOffset(xoffset, yoffset);
    }
}