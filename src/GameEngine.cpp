#include <GL/glew.h>
#include <iostream>
#include "Utils.h"
#include "ShaderPath.h"
#include "GameEngine.h"

#include "States/StateFactory.h"

// Constructor with default values
GameEngine::GameEngine()
    : window(nullptr),
    isRunning(false),
    // Need to initialize these as references from constructor, as opposed to init()
    tm(TextureManager::getInstance()),
    pm(ProfileManager::getInstance()),
    io(ImGui::GetIO()) 
{
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
}


GameEngine::~GameEngine() {
    cleanup();
}

bool GameEngine::init(const char* title, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    aspectRatio = static_cast<float>(width) / height;
    minWidth = width / 4;
    minHeight = height / 4;

    // Initialize everything
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        isRunning = false;
        return false;
    }

    // Configure GLFW BEFORE window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }

    // Multiple monitors
    monitors = glfwGetMonitors(&nMonitors);
    videoMode = glfwGetVideoMode(monitors[0]);
    glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);
    glfwSetWindowPos(window,
        monitorX + (videoMode->width - windowWidth) / 2,
        monitorY + (videoMode->height - windowHeight) / 2);


    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        isRunning = false;
        cleanup();
        return false;
    }

    // Set color blinding and depth testing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Initialize ImGui fonts
    defaultFont = io.Fonts->AddFontFromFileTTF(DEFAULT_FONT_PATH, 24.0f);
    titleFont = io.Fonts->AddFontFromFileTTF(TITLE_FONT_PATH, 48.0f);
    attackFont = io.Fonts->AddFontFromFileTTF(ATTACK_FONT_PATH, 128.0f);
    io.Fonts->Build();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    physicsWorld = new PhysicsWorld();

    // Camera and initial view initialization
    glm::vec3 initialCameraPos(5.0f, 2.4f, 0.0f);
    glm::vec3 lookAtPoint(0.0f, 1.0f, 0.0f);
    glm::vec3 frontVector = glm::normalize(lookAtPoint - initialCameraPos);
    float initialYaw = glm::degrees(std::atan2(frontVector.z, frontVector.x));
    float initialPitch = glm::degrees(std::atan2(frontVector.y, glm::length(glm::vec2(frontVector.x, frontVector.z))));

    mainCamera = Camera(initialCameraPos, initialYaw, initialPitch, 0.0f, physicsWorld);
    mainCamera.front = frontVector;
    cameraState = new CameraState(&mainCamera, 400.0, 300.0); // LOOK: Are these dimensions supposed to be hardcoded?

    model = glm::mat4(1.0f);
    view = glm::lookAt(initialCameraPos, lookAtPoint, glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
    backgroundModel = glm::mat4(1.0f);
    backgroundView = glm::mat4(1.0f);
    orthoProjection = glm::ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight), 0.0f, 1.0f);

    objectShader = new ShaderProgram(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    objectShader->setUniforms(model, view, projection);
    objectShader->setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    objectShader->setUniformVec3("lightPos", glm::vec3(0.0f, 1e5f, 0.0f));  // Light very high

    backgroundShader = new ShaderProgram(BACKGROUND_VERTEX_SHADER_PATH, BACKGROUND_FRAGMENT_SHADER_PATH);
    backgroundShader->setUniforms(backgroundModel, backgroundView, orthoProjection);
    backgroundShader->setUniform1f("wrapFactor", 4.0f);

    //    auto panoramaShader = new ShaderProgram(PANORAMA_VERTEX_SHADER_PATH, PANORAMA_FRAGMENT_SHADER_PATH);
    //    panoramaShader->setUniforms(panoramaModel, panoramaView, panoramaProjection);

    textRenderer = new TextRenderer("./assets/fonts/OpenSans-Regular.ttf", 800, 600);
    tm.loadTexture("defaultBackground", "./assets/textures/Brickbeyz.jpg");
    tm.loadTexture("sceneBackground", "./assets/textures/HexagonSmall.jpg");
    tm.loadTexture("stadium", "./assets/textures/Hexagon.jpg");
    tm.loadTexture("floor", "./assets/textures/Wood1.jpg");

    // Set default profile at first (Needed in order to not crash)
    //pm.addProfile("Default");
    pm.addDefaultProfiles();

    // Set window user pointer to GameEngine
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeLimits(window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, GameEngine::framebufferSizeCallback);  // This handles window resizing

    // Set for use in inputManager
    /**
    * Various callbacks
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
    inputManager = InputManager();
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        GameEngine* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
            engine->inputManager.setKey(key, isPressed);
        }
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        // Forward to ImGui if needed
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }

        GameEngine* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            bool isPressed = (action == GLFW_PRESS);
            engine->inputManager.setMouseButton(button, isPressed);
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        // Forward to ImGui if needed
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
        if (ImGui::GetIO().WantCaptureMouse) return;

        GameEngine* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            engine->inputManager.setMousePosition(xpos, ypos);
        }
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        // Forward to ImGui if needed
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        if (ImGui::GetIO().WantCaptureMouse) return;

        GameEngine* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
        if (engine) {
            engine->inputManager.setScrollOffset(xoffset, yoffset);
        }
    });

    // Initialize INI handling if needed
    iniFile = nullptr;
    iniData = nullptr;
    boundCamera = false;
    debugMode = false;

    prevTime = 0.0f;
    deltaTime = 0.0f;

    // LOOK: Only for testing?
    imguiColor[0] = 0.8f;
    imguiColor[1] = 0.8f;
    imguiColor[2] = 0.8f;

    quadRenderer = new QuadRenderer();

    isRunning = true;
    return true;
}

void GameEngine::initTimers() {
    // Frame rate timer - updates every second, repeats indefinitely
    timers.push_back(Timer(1.0f, true, -1.0f));
    timerCallbacks.push_back([this]() {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << "FPS: " << ImGui::GetIO().Framerate;
        fpsText = ss.str();
    });

    // Coordinate timer - updates every 0.1 seconds, repeats indefinitely
    timers.push_back(Timer(0.1f, true, -1.0f));
    timerCallbacks.push_back([this]() {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << "X: " << mainCamera.position.x << " Y: " << mainCamera.position.y << " Z: " << mainCamera.position.z;
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
        stateStack.push_back(std::move(newState));
    }
}

void GameEngine::pushState(GameStateType stateType) {
    if (!stateStack.empty()) {
        stateStack.back()->pause();
    }
    auto newState = createState(stateType);
    if (newState) {
        newState->init();
        stateStack.push_back(std::move(newState));
    }
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
        std::cout << "No states left in stack. Quitting." << std::endl;
        quit();
    }
}

std::unique_ptr<GameState> GameEngine::createState(GameStateType stateType) {
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

    if (!stateStack.empty()) {
        stateStack.back()->handleEvents();
    }

    inputManager.updateState();  // Must be called at end
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

    // Show FPS for performance
    std::stringstream ss; 
    ss << std::fixed << std::setprecision(1) << "FPS: " << ImGui::GetIO().Framerate;
    std::string fpsText = ss.str();
    textRenderer->resize(windowWidth, windowHeight);
    textRenderer->renderText(fpsText, 10.0f, windowHeight - 20.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Show coordinates if in beyblade space
    if (stateStack.back()->getStateType() == GameStateType::ACTIVE) {
        textRenderer->renderText(coordsText, 10.0f, 20.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // END IMGUI FRAME: renders to screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}


void GameEngine::cleanup() {
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
    delete cameraState;

    delete iniData;
    delete iniFile;

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

    std::cout << "GameEngine cleaned up successfully." << std::endl;
}

/**
* Callback function to adjust the viewport when the window is resized.
*/

/*static*/
void GameEngine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
    if (!engine || (width == engine->windowWidth && height == engine->windowHeight)) {
        return;
    }

    engine->windowWidth = width;
    engine->windowHeight = height;

    int newWidth = width;
    int newHeight = static_cast<int>(width / engine->aspectRatio);
    if (newHeight > height) {
        newHeight = height;
        newWidth = static_cast<int>(height * engine->aspectRatio);
    }
    if (newWidth != width || newHeight != height) {
        glfwSetWindowSize(window, newWidth, newHeight);
    }

    glViewport(0, 0, newWidth, newHeight);
    engine->projection = glm::perspective(glm::radians(45.0f), (float)newWidth / newHeight, 0.1f, 100.0f);
    engine->objectShader->use();
    engine->objectShader->setUniformMat4("projection", engine->projection);
}

/*------------------------------------Statis input callbacks-----------------------------------------------------------*/

// Static input callbacks
//void GameEngine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
//    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
//    if (engine) {
//        bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
//        engine->inputManager.setKey(key, isPressed);
//        engine->inputManager.setModifiers(mods);
//    }
//}
//
//void GameEngine::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
//    if (engine) {
//        bool isPressed = (action == GLFW_PRESS);
//        engine->inputManager.setMouseButton(button, isPressed);
//    }
//}
//
//void GameEngine::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos) {
//    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
//    if (engine) {
//        engine->inputManager.setMousePosition(xPos, yPos);
//    }
//}
//
//void GameEngine::scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
//    auto* engine = static_cast<GameEngine*>(glfwGetWindowUserPointer(window));
//    if (engine) {
//        engine->inputManager.setScrollOffset(xOffset, yOffset);
//    }
//}