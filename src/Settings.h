
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
        if (json.contains("volume")) volume = json["volume"];
        if (json.contains("isMuted")) isMuted = json["isMuted"];
        if (json.contains("resolutionIndex")) resolutionIndex = json["resolutionIndex"];
        if (json.contains("isFullscreen")) isFullscreen = json["isFullscreen"];
    }
};