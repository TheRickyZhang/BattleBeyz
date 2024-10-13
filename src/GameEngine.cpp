#include <GL/glew.h>
#include <iostream>
#include "Utils.h"
#include "ShaderPath.h"
#include "GameEngine.h"

#include "States/StateFactory.h"
#include "Callbacks.h"

// Constructor with default values (can be overridden by init)
GameEngine::GameEngine()
    : window(nullptr),
    isRunning(false),
    io(ImGui::GetIO())  // Need to initialize ImGuiIO as a reference, so need constructor
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
    mainCamera.Front = frontVector;
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

    hexagonPattern = new Texture("./assets/images/Hexagon.jpg", "texture1");
    smallHexagonPattern = new Texture("./assets/images/HexagonSmall.jpg", "texture1");
    homeScreenTexture = new Texture("./assets/images/Brickbeyz.jpg", "texture1");
    backgroundTexture = new Texture("./assets/images/Brickbeyz.jpg", "texture1");
    floorTexture = new Texture("./assets/images/Wood1.jpg", "texture1");
    stadiumTexture = new Texture("./assets/images/Hexagon.jpg", "texture1");

    // Set window user pointer to GameEngine
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeLimits(window, minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Initialize INI handling if needed
    iniFile = nullptr;
    iniData = nullptr;
    boundCamera = false;
    debugMode = false;
    selectedBeyblade = -1;


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
        ss << std::fixed << std::setprecision(1) << "X: " << mainCamera.Position.x << " Y: " << mainCamera.Position.y << " Z: " << mainCamera.Position.z;
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
        quit(); // No more states, quit the game
    }
}

std::unique_ptr<GameState> GameEngine::createState(GameStateType stateType) {
    return StateFactory::createState(stateType);
}

void GameEngine::handleEvents() {
    // Poll GLFW events (handles window events, input events, etc.)
    glfwPollEvents();

    if (!stateStack.empty()) {
        stateStack.back()->handleEvents(this);
    }
}

void GameEngine::update() {
    if (!stateStack.empty()) {
        stateStack.back()->update(this, deltaTime);
    }
}

void GameEngine::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear both color and depth buffers

    // Start the ImGui frame (applies to all states)
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Draw the current state
    if (!stateStack.empty()) {
        stateStack.back()->draw(this);
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

    // Render and swap buffers to display
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

    delete hexagonPattern;
    delete smallHexagonPattern;
    delete homeScreenTexture;
    delete backgroundTexture;
    delete floorTexture;
    delete stadiumTexture;

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
