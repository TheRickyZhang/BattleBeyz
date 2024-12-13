#include "BeybladeConstants.h"
#include "BeybladeBody.h"
#include "imgui.h"
#include <cmath>

using namespace std;

string BeybladeParameter::nameWithUnits() const {
    if (unit.empty()) return name;
    else return name + "(" + unit + ")";
}

string BeybladeParameter::getDisplayFormat() const {
    return "%." + std::to_string(stepExponent) + "f"; // For use in ImGui's "%.3f"
}

float BeybladeParameter::getStepSize() const {
    return float(pow(10.0f, -stepExponent));
}

float BeybladeParameter::getFastStepSize() const {
    return getStepSize() * 10;
}

void BeybladeParameter::assignToBeybladeBody(BeybladeBody* body) {
    body->setLayerMass(layerParameters[0].currentValue);
    body->setLayerMomentOfInertia(layerParameters[1].currentValue);
    body->setLayerCoefficientOfRestitution(layerParameters[2].currentValue);
    body->setLayerRecoilDistribution(layerParameters[3].currentValue, layerParameters[4].currentValue);

    body->setDiscMass(discParameters[0].currentValue);
    body->setDiscMomentOfInertia(discParameters[1].currentValue);

    body->setDriverMass(driverParameters[0].currentValue);
    body->setDriverMomentOfInertia(driverParameters[1].currentValue);
    body->setDriverCOF(driverParameters[2].currentValue);

    body->updateFromParts();
}

void BeybladeParameter::assignFromBeybladeBody(BeybladeBody* body) {
    layerParameters[0].currentValue = body->getLayerMass();
    layerParameters[1].currentValue = body->getLayerMomentOfInertia();
    layerParameters[2].currentValue = body->getLayerCOR();
    layerParameters[3].currentValue = body->getLayerRecoilDistribution().getMean();
    layerParameters[4].currentValue = body->getLayerRecoilDistribution().getStdDev();

    discParameters[0].currentValue = body->getDiscMass();
    discParameters[1].currentValue = body->getDiscMomentOfInertia();

    driverParameters[0].currentValue = body->getDriverMass();
    driverParameters[1].currentValue = body->getDriverMomentOfInertia();
    driverParameters[2].currentValue = body->getDriverCOF();
}

// Layer parameters
vector<BeybladeParameter> layerParameters = {
    {"Mass", "g", 1.0f, 50.0f, 15.0f, 1},
    {"Moment of Inertia", "g*m^2", 3.0e-3f, 1.6e-2f, 8.0e-3f, 4},
    {"Coefficient of Restitution", "", 0.0f, 0.5f, 0.25f, 2},
    {"Recoil Distribution Mean", "", 0.1f, 1.0f, 0.5f, 2},
    {"Recoil Distribution StdDev", "", 0.1f, 1.0f, 0.2f, 2}
};

// Disc parameters
vector<BeybladeParameter> discParameters = {
    {"Mass", "g", 1.0f, 50.0f, 25.0f, 1},
    {"Moment of Inertia", "g*m^2", 2.3e-3f, 2.025e-2f, 1.0e-2f, 4}
};

// Driver parameters
vector<BeybladeParameter> driverParameters = {
    {"Mass", "g", 1.0f, 15.0f, 5.0f, 2},
    {"Moment of Inertia", "g*m^2", 7.5e-6f, 2.0e-5f, 1.0e-5f, 7},
    {"Coefficient of Friction", "", 0.1f, 0.6f, 0.3f, 2}
};

// Cached result of max text size
static float calculateMaxTextSize(const std::vector<BeybladeParameter>& parameters) {
    float maxTextSize = 0.0f;
    for (const auto& param : parameters) {
        float textSize = ImGui::CalcTextSize(param.name.c_str()).x;
        if (textSize > maxTextSize) {
            maxTextSize = textSize;
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