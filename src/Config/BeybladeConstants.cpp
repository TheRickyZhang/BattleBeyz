#include <cmath>
#include <imgui.h>

#include "BeybladeBody.h"
#include "BeybladeConstants.h"
#include "Stadium.h"
#include "Units.h"

using namespace Units;
using namespace std;

string ScalarParameter::nameWithUnits() const {
    if (unit.empty()) return name;
    else return name + "(" + unit + ")";
}

string ScalarParameter::getDisplayFormat() const {
    return "%." + std::to_string(stepExponent) + "f"; // For use in ImGui's "%.3f"
}

float ScalarParameter::getStepSize() const {
    return float(pow(10.0f, -stepExponent));
}

float ScalarParameter::getFastStepSize() const {
    return getStepSize() * 10;
}

void ScalarParameter::assignToBeybladeBody(BeybladeBody* body) {
    body->layer->mass = Kg(layerParameters[0].currentValue);
    body->layer->momentOfInertia = KgM2(layerParameters[1].currentValue);
    body->layer->coefficientOfRestitution = Scalar(layerParameters[2].currentValue);
    body->layer->recoilDistribution = RandomDistribution(Scalar(layerParameters[3].currentValue), Scalar(layerParameters[4].currentValue));

    body->disc->mass = Kg(discParameters[0].currentValue);
    body->disc->momentOfInertia = KgM2(discParameters[1].currentValue);

    body->driver->mass = Kg(driverParameters[0].currentValue);
    body->driver->momentOfInertia = KgM2(driverParameters[1].currentValue);
    body->driver->coefficientOfFriction = Scalar(driverParameters[2].currentValue);

    body->updateFromParts();
}

void ScalarParameter::assignFromBeybladeBody(BeybladeBody* body) {
    layerParameters[0].currentValue = body->layer->mass.value();
    layerParameters[1].currentValue = body->layer->momentOfInertia.value();
    layerParameters[2].currentValue = body->layer->coefficientOfRestitution.value();
    layerParameters[3].currentValue = body->layer->recoilDistribution.getMean().value();
    layerParameters[4].currentValue = body->layer->recoilDistribution.getStdDev().value();

    discParameters[0].currentValue = body->disc->mass.value();
    discParameters[1].currentValue = body->disc->momentOfInertia.value();

    driverParameters[0].currentValue = body->driver->mass.value();
    driverParameters[1].currentValue = body->driver->momentOfInertia.value();
    driverParameters[2].currentValue = body->driver->coefficientOfFriction.value();
}

void ScalarParameter::assignToStadium(Stadium* stadium) {
    if (!stadium) return;
    stadium->setRadius(stadiumParameters[0].currentValue);
    stadium->setCurvature(stadiumParameters[1].currentValue);
    stadium->setFriction(stadiumParameters[2].currentValue);
    stadium->setVerticesPerRing((int)stadiumParameters[3].currentValue);
    stadium->setNumRings((int)stadiumParameters[4].currentValue);
}

void ScalarParameter::assignFromStadium(const Stadium* stadium) {
    if (!stadium) return;
    stadiumParameters[0].currentValue = stadium->getRadius().value();
    stadiumParameters[1].currentValue = stadium->getCurvature().value();
    stadiumParameters[2].currentValue = stadium->getCOF().value();
    stadiumParameters[3].currentValue = static_cast<float>(stadium->getVerticesPerRing());
    stadiumParameters[4].currentValue = static_cast<float>(stadium->getNumRings());
}

void Vec3Parameter::assignToStadium(Stadium* stadium) {
    if (!stadium) return;
    stadium->setTint(stadiumVec3Parameters[0].currentValue);       // Assign Tint
    stadium->setRingColor(stadiumVec3Parameters[1].currentValue); // Assign Ring Color
    stadium->setCrossColor(stadiumVec3Parameters[2].currentValue);// Assign Cross Color
}

void Vec3Parameter::assignFromStadium(const Stadium* stadium) {
    if (!stadium) return;
    stadiumVec3Parameters[0].currentValue = stadium->getTint();       // Fetch Tint
    stadiumVec3Parameters[1].currentValue = stadium->getRingColor(); // Fetch Ring Color
    stadiumVec3Parameters[2].currentValue = stadium->getCrossColor();// Fetch Cross Color
}



// Layer parameters
vector<ScalarParameter> layerParameters = {
    {"Mass", "g", 15.0f, 35.0f, 22.0f, 1},
    {"Moment of Inertia", "g*m^2", 3.0e-6f, 1.6e-5f, 8.0e-3f, 4},
    {"Coefficient of Restitution", "", 0.0f, 0.5f, 0.25f, 2},
    {"Recoil Distribution Mean", "", 0.1f, 1.0f, 0.5f, 2},
    {"Recoil Distribution StdDev", "", 0.1f, 1.0f, 0.2f, 2}
};

// Disc parameters
vector<ScalarParameter> discParameters = {
    {"Mass", "g", 1.0f, 50.0f, 25.0f, 1},
    {"Moment of Inertia", "g*m^2", 2.3e-3f, 2.025e-2f, 1.0e-2f, 4}
};

// Driver parameters
vector<ScalarParameter> driverParameters = {
    {"Mass", "g", 1.0f, 15.0f, 5.0f, 2},
    {"Moment of Inertia", "g*m^2", 7.5e-6f, 2.0e-5f, 1.0e-5f, 7},
    {"Coefficient of Friction", "", 0.1f, 0.6f, 0.3f, 2}
};

std::vector<ScalarParameter> stadiumParameters = {
    { "Radius", "m", 0.3f, 4.8f, 1.2f, 2 },
    { "Curvature", "", 0.025f, 0.4f, 0.10f, 3 },
    { "Friction (COF)", "", 0.0f, 1.0f, 0.35f, 2 },
    { "Vertices per Ring", "", 20.0f, 180.0f, 48.0f, 0 },
    { "Number of Rings", "", 4.0f, 32.0f, 8.0f, 0 }
};

std::vector<Vec3Parameter> stadiumVec3Parameters = {
    { "Tint Color", glm::vec3(1.0f, 1.0f, 1.0f) },
    { "Ring Color", glm::vec3(1.0f, 0.0f, 0.0f) },
    { "Cross Color", glm::vec3(0.0f, 0.0f, 1.0f) }
};



// Cached result of max text size
static float calculateMaxTextSize(const std::vector<ScalarParameter>& parameters) {
    float maxTextSize = 0.0f;
    for (const auto& param : parameters) {
        float textSize = ImGui::CalcTextSize(param.name.c_str()).x;
        if (textSize > maxTextSize) {
            maxTextSize = textSize;
        }
    }
    return maxTextSize;
}
static float calculateMaxTextSize(const std::vector<Vec3Parameter>& parameters) {
    float maxTextSize = 0.0f;
    for (const auto& param : parameters) {
        float textSize = ImGui::CalcTextSize(param.name.c_str()).x;

        // The default width of a color widget can vary, but we'll assume 100 pixels here for simplicity.
        float colorWidgetWidth = ImGui::GetStyle().FramePadding.x * 2 + 100.0f;
        float totalWidth = textSize + ImGui::GetStyle().ItemSpacing.x + colorWidgetWidth;
        if (totalWidth > maxTextSize) {
            maxTextSize = totalWidth;
        }
    }
    return maxTextSize;
}


float getMaxLayerTextSize() {
    static float maxTextSize = calculateMaxTextSize(layerParameters);
    return maxTextSize;
}
float getMaxDiscTextSize() {
    static float maxTextSize = calculateMaxTextSize(discParameters);
    return maxTextSize;
}
float getMaxDriverTextSize() {
    static float maxTextSize = calculateMaxTextSize(driverParameters);
    return maxTextSize;
}
float getMaxStadiumTextSize() {
    static float maxTextSize = calculateMaxTextSize(stadiumParameters);
    static float maxVec3TextSize = calculateMaxTextSize(stadiumVec3Parameters);
    return max(maxTextSize, maxVec3TextSize);
}