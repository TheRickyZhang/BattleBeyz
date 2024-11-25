////////////////////////////////////////////////////////////////////////////////
// GameControl.h -- Game Control Datas -- rz -- 2024-08-08
// Copyright (c) 2024, Ricky Zhang.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Camera.h"
#include "QuadRenderer.h"
#include "PhysicsWorld.h"
#include "imgui.h"
#include "Utils.h"

#define MINI_CASE_SENSITIVE
#include "mini/ini.h"


// All data needed to be passed to the callback functions
struct GameControl {
    int* windowWidth;
    int* windowHeight;
    float aspectRatio;
    glm::mat4* projection;
    ShaderProgram* shader;
    ShaderProgram* backgroundShader;
    QuadRenderer* quadRenderer;
    bool showHomeScreen;
    bool showInfoScreen;
    bool showCustomizeScreen;
    bool showAboutScreen;
    ImFont* defaultFont;
    ImFont* titleFont;
    ImFont* attackFont;
    bool boundCamera;
    ProgramState currentState;
    bool debugMode;
    float deltaTime;
    PhysicsWorld* physicsWorld;

    int selectedBeyblade = -1;
    bool showOptionsScreen = false;

    // NEWUI
    // For ini file handling

    mINI::INIFile* iniFile;
    mINI::INIStructure* iniData;

    GameControl(int* width, int* height, float ratio, glm::mat4* proj, ShaderProgram* sh, ShaderProgram* background,
        QuadRenderer* quadRend, bool showHome, bool showInfo,
        bool showCustomize, bool showAbout, ImFont* defaultF, ImFont* titleF, ImFont* attackF,
        bool boundCam, ProgramState programState, bool _debugMode, PhysicsWorld* physicsWorld)
        : windowWidth(width),
        windowHeight(height),
        aspectRatio(ratio),
        projection(proj),
        shader(sh),
        backgroundShader(background),
        quadRenderer(quadRend),
        showHomeScreen(showHome),
        showInfoScreen(showInfo),
        showCustomizeScreen(showCustomize),
        showAboutScreen(showAbout),
        defaultFont(defaultF),
        titleFont(titleF),
        attackFont(attackF),
        boundCamera(false),
        currentState(programState),
        debugMode(_debugMode),
        physicsWorld(physicsWorld)
    {
        deltaTime = 0.0f;

        // These will be allocated in the customization code on first use.

        iniFile = nullptr;
        iniData = nullptr;
    }

    ~GameControl() {
        delete iniData;
        delete iniFile;
    }
};