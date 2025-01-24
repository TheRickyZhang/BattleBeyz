#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

class BeybladeBody;
class Stadium;
struct ScalarParameter {
    std::string name;
    std::string unit;
    float minValue;
    float maxValue;
    float defaultValue;
    int stepExponent;

    float currentValue = 0.0; // Temporary value for the slider; make sure to sync with beyblade values

    std::string nameWithUnits() const;
    std::string getDisplayFormat() const;
    float getStepSize() const;
    float getFastStepSize() const;

    static void assignToBeybladeBody(BeybladeBody* body);
    static void assignFromBeybladeBody(BeybladeBody* body);

    static void assignToStadium(Stadium* stadium);
    static void assignFromStadium(const Stadium* stadium);
};

struct Vec3Parameter {
    std::string name;
    glm::vec3 defaultValue;
    glm::vec3 currentValue;
    Vec3Parameter(const std::string& name, const glm::vec3& defaultValue)
        : name(name), defaultValue(defaultValue), currentValue(defaultValue) {
    }
    static void assignToStadium(Stadium* stadium);
    static void assignFromStadium(const Stadium* stadium);
    void draw() {
        ImGui::ColorEdit3(name.c_str(), glm::value_ptr(currentValue));
    }
};

// Vectors to hold parameters for each component
extern std::vector<ScalarParameter> layerParameters;
extern std::vector<ScalarParameter> discParameters;
extern std::vector<ScalarParameter> driverParameters;

extern std::vector<ScalarParameter> stadiumParameters;
extern std::vector<Vec3Parameter> stadiumVec3Parameters;

float getMaxLayerTextSize();
float getMaxDiscTextSize();
float getMaxDriverTextSize();
float getMaxStadiumTextSize();