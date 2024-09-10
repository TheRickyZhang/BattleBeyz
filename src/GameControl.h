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


// All data needed to be passed to the callback functions
struct GameControl {
    int *windowWidth;
    int *windowHeight;
    float aspectRatio;
    glm::mat4 *projection;
    ShaderProgram *shader;
    ShaderProgram* backgroundShader;
    CameraState *cameraState;
    QuadRenderer *quadRenderer;
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

    GameControl(int *width, int *height, float ratio, glm::mat4 *proj, ShaderProgram *sh, ShaderProgram* background,
                 CameraState *camState, QuadRenderer *quadRend, bool showHome, bool showInfo,
                 bool showCustomize, bool showAbout, ImFont* defaultF, ImFont* titleF, ImFont* attackF,
                 bool boundCam, ProgramState programState, bool _debugMode, PhysicsWorld* physicsWorld)
            : windowWidth(width),
              windowHeight(height),
              aspectRatio(ratio),
              projection(proj),
              shader(sh),
              backgroundShader(background),
              cameraState(camState),
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
        deltaTime = 0.0f;  // This is updated in the main loop.
}
};