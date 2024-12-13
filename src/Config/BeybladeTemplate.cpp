#include "BeybladeTemplate.h"
#include "BeybladeParts.h"
#include "BeybladeTemplatePath.h"

// Define templateLayers with TemplateFormat<Layer>
std::vector<TemplateFormat<Layer>> templateLayers = {
    {"Standard", LAYER_STANDARD_PATH, Layer(0.025, 0.01, 0.3, 0.1, 0.4, 0.9, 0.022, glm::vec3(0.0f), glm::vec3(0.0f), 1.2e-5)},
    {"Wide",     LAYER_WIDE_PATH,     Layer(0.03,  0.01, 0.35, 0.12, 0.4, 1.0, 0.024, glm::vec3(0.0f), glm::vec3(0.0f), 1.6e-5)},
    {"Tall",     LAYER_TALL_PATH,     Layer(0.025, 0.012, 0.28, 0.1,  0.5, 1.1, 0.023, glm::vec3(0.0f), glm::vec3(0.0f), 1.3e-5)},
    {"Light",    LAYER_LIGHT_PATH,    Layer(0.025, 0.01, 0.3,  0.08, 0.3, 0.8, 0.018, glm::vec3(0.0f), glm::vec3(0.0f), 1.0e-5)},
    {"Heavy",    LAYER_HEAVY_PATH,    Layer(0.025, 0.01, 0.4,  0.15, 0.6, 1.2, 0.035, glm::vec3(0.0f), glm::vec3(0.0f), 1.8e-5)}
};

// Define templateDiscs with TemplateFormat<Disc>
std::vector<TemplateFormat<Disc>> templateDiscs = {
    {"Standard", DISC_STANDARD_PATH, Disc(0.018, 0.01, 0.027, 0.7*0.027*0.018*0.018, 0.1*0.005)},
    {"Wide",     DISC_WIDE_PATH,     Disc(0.03,  0.01, 0.030, 0.7*(0.030*0.03*0.03), 0.12*0.006)},
    {"Heavy",    DISC_HEAVY_PATH,    Disc(0.02,  0.012,0.035, 0.8*(0.035*0.02*0.02), 0.15*0.007)},
    {"Light",    DISC_LIGHT_PATH,    Disc(0.015, 0.008,0.020, 0.6*(0.020*0.015*0.015), 0.08*0.004)},
    {"Compact",  DISC_COMPACT_PATH,  Disc(0.017, 0.01, 0.025, 0.65*(0.025*0.017*0.017), 0.09*0.0045)}
};

// Define templateDrivers with TemplateFormat<Driver>
std::vector<TemplateFormat<Driver>> templateDrivers = {
    {"Standard",       DRIVER_STANDARD_PATH,       Driver(0.002, 0.015, 0.005, 0.5*(0.005*0.002*0.0015), 0.1*0.005, 0.22)},
    {"Wide",           DRIVER_WIDE_PATH,           Driver(0.0025,0.015, 0.006, 0.5*(0.006*0.0025*0.0015),0.12*0.006, 0.25)},
    {"Tall",           DRIVER_TALL_PATH,           Driver(0.002, 0.02,  0.007, 0.6*(0.007*0.002*0.002), 0.1*0.007, 0.20)},
    {"Light",          DRIVER_LIGHT_PATH,          Driver(0.0015,0.01,  0.004, 0.4*(0.004*0.0015*0.001),0.08*0.004, 0.18)},
    {"High Friction",  DRIVER_HIGH_FRICTION_PATH,  Driver(0.002, 0.015, 0.005, 0.5*(0.005*0.002*0.0015),0.1*0.005, 0.35)}
};
