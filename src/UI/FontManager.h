#pragma once

#include <unordered_map>
#include <string>
#include <imgui.h>

class FontManager {
public:
    static FontManager& getInstance() {
        static FontManager instance;
        return instance;
    }

    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    void loadFont(const std::string& name, const std::string& filePath, float size);
    ImFont* getFont(const std::string& name) const;

    void loadDefaultFonts();
    void clearFonts();

private:
    FontManager() = default;

    std::unordered_map<std::string, ImFont*> fonts;
};