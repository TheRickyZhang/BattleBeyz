#include <filesystem>
#include <iostream>
#include <fstream>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <json.hpp>

#include "FontManager.h"
#include "MessageLog.h"
#include "ShaderPath.h"

using namespace std;
using namespace nlohmann;

// TOLOOK: Move somewhere else and expand when more is added to default profile
void createDefaultProfile() {
    json defaultProfile = {
        {"id", 2},
        {"name", "Hello"},
        {"settings", {
            {"isFullscreen", false},
            {"isMuted", false},
            {"resolutionIndex", 1},
            {"volume", 0.5}
        }}
    };
    ofstream outFile(PROFILE_SAVE_PATH);
    outFile << defaultProfile.dump(4);
    outFile.close();
}

void setupImGui(GLFWwindow* window) {
    using namespace filesystem;
    MessageLog& ml = MessageLog::getInstance();

    // Ensure ImGui version and context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Set config and logging locations
    io.IniFilename = IMGUI_SAVE_PATH;
    io.LogFilename = nullptr; // Avoid ImGui for now

    // Ensure directories exist or create them
    if (!exists("./game_data")) {
        create_directories("./game_data");
        ml.addMessage("Created directory: ./game_data");
    }
    if (!exists(PROFILE_SAVE_PATH)) {
        createDefaultProfile();
    }
    if (!exists(IMGUI_SAVE_PATH)) {
        ml.addMessage("Warning: Config file " + string(IMGUI_SAVE_PATH) + " not found.Using default settings.", MessageType::ERROR, true);
    }

    //ImGui::StyleColorsDark(); // Replace with a call to your custom style function if needed
    // setupCustomStyles();  // Example: Call a custom style setup function

    ml.addMessage("ImGui setup complete. Config and log files are set.");
}