#include "BeybladeTemplate.h"
#include "BeybladeParts.h"
#include "BeybladeTemplatePath.h"

// Define templateLayers with TemplateFormat<Layer>
std::vector<TemplateFormat<Layer>> templateLayers = {
    {"Standard", LAYER_STANDARD_PATH, Layer(0.025f, 0.01f, 0.3f, 0.1f, 0.4f, 0.9f, 0.022f, glm::vec3(0.0f), glm::vec3(0.0f), 1.2e-5f)},
    {"Wide",     LAYER_WIDE_PATH,     Layer(0.03f,  0.01f, 0.35f, 0.12f, 0.4f, 1.0f, 0.024f, glm::vec3(0.0f), glm::vec3(0.0f), 1.6e-5f)},
    {"Tall",     LAYER_TALL_PATH,     Layer(0.025f, 0.012f, 0.28f, 0.1f,  0.5f, 1.1f, 0.023f, glm::vec3(0.0f), glm::vec3(0.0f), 1.3e-5f)},
    {"Light",    LAYER_LIGHT_PATH,    Layer(0.025f, 0.01f, 0.3f,  0.08f, 0.3f, 0.8f, 0.018f, glm::vec3(0.0f), glm::vec3(0.0f), 1.0e-5f)},
    {"Heavy",    LAYER_HEAVY_PATH,    Layer(0.025f, 0.01f, 0.4f,  0.15f, 0.6f, 1.2f, 0.035f, glm::vec3(0.0f), glm::vec3(0.0f), 1.8e-5f)}
};

// Define templateDiscs with TemplateFormat<Disc>
std::vector<TemplateFormat<Disc>> templateDiscs = {
    {"Standard", DISC_STANDARD_PATH, Disc(0.018f, 0.01f, 0.027f, 0.7f * 0.027f * 0.018f * 0.018f, 0.1f * 0.005f)},
    {"Wide",     DISC_WIDE_PATH,     Disc(0.03f,  0.01f, 0.030f, 0.7f * (0.030f * 0.03f * 0.03f), 0.12f * 0.006f)},
    {"Heavy",    DISC_HEAVY_PATH,    Disc(0.02f,  0.012f, 0.035f, 0.8f * (0.035f * 0.02f * 0.02f), 0.15f * 0.007f)},
    {"Light",    DISC_LIGHT_PATH,    Disc(0.015f, 0.008f, 0.020f, 0.6f * (0.020f * 0.015f * 0.015f), 0.08f * 0.004f)},
    {"Compact",  DISC_COMPACT_PATH,  Disc(0.017f, 0.01f, 0.025f, 0.65f * (0.025f * 0.017f * 0.017f), 0.09f * 0.0045f)}
};

// Define templateDrivers with TemplateFormat<Driver>
std::vector<TemplateFormat<Driver>> templateDrivers = {
    {"Standard",       DRIVER_STANDARD_PATH,       Driver(0.002f, 0.015f, 0.005f, 0.5f * (0.005f * 0.002f * 0.0015f), 0.1f * 0.005f, 0.22f)},
    {"Wide",           DRIVER_WIDE_PATH,           Driver(0.0025f, 0.015f, 0.006f, 0.5f * (0.006f * 0.0025f * 0.0015f), 0.12f * 0.006f, 0.25f)},
    {"Tall",           DRIVER_TALL_PATH,           Driver(0.002f, 0.02f,  0.007f, 0.6f * (0.007f * 0.002f * 0.002f), 0.1f * 0.007f, 0.20f)},
    {"Light",          DRIVER_LIGHT_PATH,          Driver(0.0015f, 0.01f,  0.004f, 0.4f * (0.004f * 0.0015f * 0.001f), 0.08f * 0.004f, 0.18f)},
    {"High Friction",  DRIVER_HIGH_FRICTION_PATH,  Driver(0.002f, 0.015f, 0.005f, 0.5f * (0.005f * 0.002f * 0.0015f), 0.1f * 0.005f, 0.35f)}
};
