#include "MessageLog.h"
#include <nlohmann/json.hpp>

struct Settings {
    float volume = 0.5f;
    bool isMuted = false;
    int resolutionIndex = 1; // Default: 1280x720
    bool isFullscreen = false;

    nlohmann::json toJson() const {
        return {
            { "volume", volume },
            { "isMuted", isMuted },
            { "resolutionIndex", resolutionIndex },
            { "isFullscreen", isFullscreen }
        };
    }

    void fromJson(const nlohmann::json& json) {
        MessageLog& ml = MessageLog::getInstance();

        // Validate and log issues; set defaults if necessary
        volume = (json.contains("volume") && json["volume"].is_number())
            ? json["volume"].get<float>()
            : (ml.addMessage("'volume' is missing or not a valid number. Using default value: 1.0", MessageType::Warning), 1.0f);

        isMuted = (json.contains("isMuted") && json["isMuted"].is_boolean())
            ? json["isMuted"].get<bool>()
            : (ml.addMessage("'isMuted' is missing or not a valid boolean. Using default value: false", MessageType::Warning), false);

        resolutionIndex = (json.contains("resolutionIndex") && json["resolutionIndex"].is_number_integer())
            ? json["resolutionIndex"].get<int>()
            : (ml.addMessage("'resolutionIndex' is missing or not a valid integer. Using default value: 0", MessageType::Warning), 0);

        isFullscreen = (json.contains("isFullscreen") && json["isFullscreen"].is_boolean())
            ? json["isFullscreen"].get<bool>()
            : (ml.addMessage("'isFullscreen' is missing or not a valid boolean. Using default value: false", MessageType::Warning), false);
    }
};