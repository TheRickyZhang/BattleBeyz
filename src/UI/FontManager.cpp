#include "FontManager.h"
#include "ShaderPath.h"
#include <stdexcept>
#include <iostream>

void FontManager::loadFont(const std::string& name, const std::string& filePath, float size) {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF(filePath.c_str(), size);
    if (!font) {
        throw std::runtime_error("Failed to load font: " + filePath);
    }
    fonts[name] = font;

    std::cout << "Font loaded: " << name << " from " << filePath << std::endl;
}

ImFont* FontManager::getFont(const std::string& name) const {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }

    throw std::runtime_error("Font not found: " + name);
}

void FontManager::loadDefaultFonts() {
    loadFont("default", DEFAULT_FONT_PATH, 24.0f);
    loadFont("title", TITLE_FONT_PATH, 48.0f);
    loadFont("attack", ATTACK_FONT_PATH, 128.0f);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Build();
}

void FontManager::clearFonts() {
    fonts.clear();  // ImGui manages font memory, so no manual deletion is necessary.
}
