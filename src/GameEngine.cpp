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
#include "ImGuiUI.h"
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

    // Basic setup with GFLW, window, and GLEW
    if (!initializeGLFW()) {
        isRunning = false;
        return false;
    }
    if (!createWindow(title, width, height)) {
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }
    if (!initializeGLEW()) {
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }
    glViewport(0, 0, windowWidth, windowHeight);

    setupImGuiAndCallbacks();

    // Largest change of crashing on json save data import
    if (!attemptSaveDataLoad()) {
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

    // Physics/space related
    physicsWorld = new PhysicsWorld();
    initCamera();
    initShaders();
    initRenderers();

    // Other
    initTimers();
    GameState::initStyleParams();

    boundCamera = false;
    debugMode = false;
    isRunning = true;

    return true;
}

/**
* Retrieves the current game state.
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

void GameEngine::changeState(std::unique_ptr<GameState> newState) {
    // Clear the entire stack
    while (!stateStack.empty()) {
        stateStack.back()->cleanup();
        stateStack.pop_back();
    }

    // Initialize and push the new state
    if (newState) {
        newState->init();
        stateStack.push_back(std::move(newState));
    }
}

/**
* Adds a customizable GameState with more detail than the generic factory function
*/
void GameEngine::pushState(std::unique_ptr<GameState> state) {
    if (!stateStack.empty()) {
        stateStack.back()->pause();
    }
    stateStack.push_back(std::move(state));
    if (!stateStack.empty()) stateStack.back()->init();
}

/**
* Performs cleanup on the current state, then pops it
* 
* Resumes the next state if it exists.
*/
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

/**
* Handles polling with rendering events and device input
*
* Updates input manager state at en
*/
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

/**
* Updates all master time variables usting glfwGetTime()
*
* Updates timers correspondingly and calls update() with the deltaTime
*/
void GameEngine::update() {
    // Update the time
#if 1
    currTime = static_cast<float>(glfwGetTime());
    deltaTime = currTime - prevTime;
    prevTime = currTime;
# else
    deltaTime = 0.0052f; // fixed frame rate, lower = slower
#endif

    updateTimers(currTime);

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
        drawDebugScreen();
    }

    // END IMGUI FRAME: renders to screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}


/**
* BEGIN PRIVATE
*
* Implementation of helper funcitons within class
*/

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

void GameEngine::updateTimers(float currentTime) {
    while (!timers.empty()) {
        Timer topTimer = timers.top();

        if (topTimer.shouldTrigger(currentTime)) {
            timers.pop();
            topTimer.trigger(currentTime);
            if (topTimer.isActive()) {
                timers.push(topTimer);
            }
        }
        else {
            break;
        }
    }
}

void GameEngine::drawDebugScreen()
{
    //// Render faded background
    //ImDrawList* drawList = ImGui::GetForegroundDrawList();
    //drawList->AddRectFilled(
    //    ImVec2(0, 0),
    //    ImVec2(float(windowWidth), float(windowHeight)),
    //    IM_COL32(0, 0, 0, 150)
    //);

    ImGui::SetNextWindowFocus(); // Set to front
    ImGui::Begin("Debug Screen", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Exit by pressing F3");
    ImGui::Text(fpsText.c_str());
    ImGui::Text("Window Size: %dx%d", windowWidth, windowHeight);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    ImGui::Text("Mouse Position: (%.1f, %.1f)", mouseX, mouseY);
    ImGui::Text(coordsText.c_str());
    ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));

    ImGui::Text("Profiles:");
    auto activeProfile = pm.getActiveProfile();
    for (auto& p : pm.getAllProfiles()) {
        if (p == activeProfile) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), p->getName().c_str());
        }
        else {
            ImGui::Text(p->getName().c_str());
        }
    }

    if (activeProfile) {
        auto activeBeyblade = activeProfile->getActiveBeyblade();
        for (auto& beyblade : activeProfile->getAllBeyblades()) {
            if (beyblade == activeBeyblade) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), beyblade->getName().c_str());
            }
            else {
                ImGui::Text(beyblade->getName().c_str());
            }
        }

        ImGui::Text("Beyblade Information");
        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << activeBeyblade->getBody()->layer->radius.value() << ", "
            << activeBeyblade->getBody()->layer->mass.value();
        ImGui::Text(oss.str().c_str());

        auto activeStadium = activeProfile->getActiveStadium();
        for (auto& stadium : activeProfile->getAllStadiums()) {
            if (stadium == activeStadium) {
                ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), stadium->getName().c_str());
            }
            else {
                ImGui::Text(stadium->getName().c_str());
            }
        }

        ImGui::Text("Stadium Information");
        std::ostringstream stadiumOss;
        stadiumOss.precision(2);
        stadiumOss << std::fixed
            << "Radius: " << activeStadium->getRadius().value() << " m, "
            << "Vertices Per Ring: " << activeStadium->getVerticesPerRing();
        ImGui::Text(stadiumOss.str().c_str());
    }
    else {
        ImGui::Text("No active profile selected");
    }

    ImGui::End();
}


/**
* Immediate helper functions to init()
*
* Returns bool if there is a possibility of failing, otherwise return void.
*/

bool GameEngine::initializeGLFW()
{
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    return true;
}

bool GameEngine::createWindow(const char* title, int width, int height)
{
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create window" << endl;
        return false;
    }

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

    return true;
}

bool GameEngine::initializeGLEW()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    while (glGetError() != GL_NO_ERROR) {}  // Do not remove, flushes error buffer
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void GameEngine::setupImGuiAndCallbacks()
{
    setupImGui(window);

    // Since these callbacks also forward to ImGui, we declare them after
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

bool GameEngine::attemptSaveDataLoad()
{
    // Catch any errors from file importing, returning a blue screen of informational death if failing
    try {
        // TODO: Add default profiles instead of crashing if json not parsed correctly
        pm.loadProfilesFromFile(PROFILE_SAVE_PATH);
        return true;
    }
    catch (const nlohmann::json::exception& e) {
        //pm.addDefaultProfiles();

        // Add critical error messages to the message log
        ml.addMessage("Critical error during initialization: " + string(e.what()), MessageType::ERROR);
        ml.addMessage("Please investigate the issue, and reach out to the contact email for further support");
        ml.open();
    }
    catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }
    catch (...) {
        cerr << "An unknown error occurred during initialization." << endl;
    }
    return false;
}

void GameEngine::initCamera()
{
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
}

void GameEngine::initShaders()
{
    backgroundShader = new BackgroundShader(BACKGROUND_VERTEX_SHADER_PATH, BACKGROUND_FRAGMENT_SHADER_PATH);
    backgroundShader->setBackgroundGlobalParams(projection, 4.0, float(glfwGetTime()));

    objectShader = new ObjectShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    objectShader->setGlobalRenderParams(view, projection, camera->position);
    // TODO: Remove this? 
    objectShader->setObjectRenderParams(model, glm::vec3(1.0f));
    objectShader->setLight(LightType::Directional, vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));
}

void GameEngine::initRenderers() {
    quadRenderer = new QuadRenderer();

    textRenderer = new TextRenderer("./assets/fonts/OpenSans-Regular.ttf", 800, 600);
    tm.loadTexture("defaultBackground", "./assets/textures/Brickbeyz.jpg");
    tm.loadTexture("sceneBackground", "./assets/textures/HexagonSmall.jpg");
    tm.loadTexture("stadium", "./assets/textures/Hexagon.jpg");
    tm.loadTexture("floor", "./assets/textures/Wood1.jpg");
    fm.loadDefaultFonts();
}


void GameEngine::initTimers() {
    // Frame rate timer
    timers.push(Timer(0.5f, [this]() {
        stringstream ss;
        ss << fixed << setprecision(1) << "FPS: " << ImGui::GetIO().Framerate;
        fpsText = ss.str();
        }, true, -1.0f));

    // Coordinate timer - updates every 0.1 seconds, repeats indefinitely
    timers.push(Timer(0.1f, [this]() {
        stringstream ss;
        ss << fixed << setprecision(1) << "X: " << camera->position.x << " Y: " << camera->position.y << " Z: " << camera->position.z;
        coordsText = ss.str();
        }, true, -1.0f));

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
    glViewport(0, 0, width, height);

    engine->onResize(width, height);
    if (!engine->stateStack.empty()) {
        engine->stateStack.back()->onResize(width, height);
    }

    engine->projection = perspective(radians(45.0f), (float)width / height, 0.1f, 100.0f);
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

// Everything that is part of engine on resize
void GameEngine::onResize(int width, int height)
{
    textRenderer->resize(width, height);
    //quadRenderer->resize(width, height);
}
