#pragma once

#include <string>
#include <vector>

class BeybladeBody;
struct BeybladeParameter {
    std::string name;
    std::string unit;
    float minValue;
    float maxValue;
    float defaultValue;
    int stepExponent;

    float currentValue = 0.0f; // Temporary value for the slider; make sure to sync with beyblade values

    std::string nameWithUnits() const;
    std::string getDisplayFormat() const;
    float getStepSize() const;
    float getFastStepSize() const;

    static void assignToBeybladeBody(BeybladeBody* body);
    static void assignFromBeybladeBody(BeybladeBody* body);
};

// Vectors to hold parameters for each component
extern std::vector<BeybladeParameter> layerParameters;
extern std::vector<BeybladeParameter> discParameters;
extern std::vector<BeybladeParameter> driverParameters;

float getMaxLayerTextSize();
float getMaxDiscTextSize();
float getMaxDriverTextSize();