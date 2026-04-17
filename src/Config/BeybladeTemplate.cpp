#include "BeybladeTemplate.h"
#include "BeybladeParts.h"
#include "BeybladeTemplatePath.h"

#include "Units.h"
using namespace Units;

// Define templateLayers with TemplateFormat<Layer>
std::vector<TemplateFormat<Layer>> templateLayers = {
    {"Standard", LAYER_STANDARD_PATH, std::make_shared<Layer>(0.025_m, 0.01_m,  0.3_sc,  0.1_sc,  0.4_sc, 0.9_sc, 0.022_kg, 1.2e-5_kg_m2)},
    {"Wide",     LAYER_WIDE_PATH,     std::make_shared<Layer>(0.03_m,  0.01_m,  0.35_sc, 0.12_sc, 0.4_sc, 1.0_sc, 0.024_kg, 1.6e-5_kg_m2)},
    {"Tall",     LAYER_TALL_PATH,     std::make_shared<Layer>(0.025_m, 0.012_m, 0.28_sc, 0.1_sc,  0.5_sc, 1.1_sc, 0.023_kg, 1.3e-5_kg_m2)},
    {"Light",    LAYER_LIGHT_PATH,    std::make_shared<Layer>(0.025_m, 0.01_m,  0.3_sc,  0.08_sc, 0.3_sc, 0.8_sc, 0.018_kg, 1.0e-5_kg_m2)},
    {"Heavy",    LAYER_HEAVY_PATH,    std::make_shared<Layer>(0.025_m, 0.01_m,  0.4_sc,  0.15_sc, 0.6_sc, 1.2_sc, 0.035_kg, 1.8e-5_kg_m2)}
};

// Define templateDiscs with TemplateFormat<Disc>
std::vector<TemplateFormat<Disc>> templateDiscs = {
    {"Standard", DISC_STANDARD_PATH, std::make_shared<Disc>(0.018_m, 0.01_m,  0.027_kg, 0.7f * 0.027_kg * 0.018_m * 0.018_m,  0.1_sc * 0.005f)},
    {"Wide",     DISC_WIDE_PATH,     std::make_shared<Disc>(0.03_m,  0.01_m,  0.030_kg, 0.7f * 0.030_kg * 0.03_m * 0.03_m,    0.12_sc * 0.006f)},
    {"Heavy",    DISC_HEAVY_PATH,    std::make_shared<Disc>(0.02_m,  0.012_m, 0.035_kg, 0.8f * 0.035_kg * 0.02_m * 0.02_m,    0.15_sc * 0.007f)},
    {"Light",    DISC_LIGHT_PATH,    std::make_shared<Disc>(0.015_m, 0.008_m, 0.020_kg, 0.6f * 0.020_kg * 0.015_m * 0.015_m,  0.08_sc * 0.004f)},
    {"Compact",  DISC_COMPACT_PATH,  std::make_shared<Disc>(0.017_m, 0.01_m,  0.025_kg, 0.65f * 0.025_kg * 0.017_m * 0.017_m, 0.09_sc * 0.0045f)}
};

// Define templateDrivers with TemplateFormat<Driver>
std::vector<TemplateFormat<Driver>> templateDrivers = {
    {"Standard",       DRIVER_STANDARD_PATH,       std::make_shared<Driver>(0.002_m, 0.012_m,  0.015_m, 0.005_kg, 0.5f * 0.005_kg * 0.002_m * 0.0015_m,  0.1_sc * 0.005f,  0.22_sc)},
    {"Wide",           DRIVER_WIDE_PATH,           std::make_shared<Driver>(0.0025_m, 0.012_m, 0.015_m, 0.006_kg, 0.5f * 0.006_kg * 0.0025_m * 0.0015_m, 0.12_sc * 0.006f, 0.25_sc)},
    {"Tall",           DRIVER_TALL_PATH,           std::make_shared<Driver>(0.002_m,  0.012_m, 0.02_m,  0.007_kg, 0.6f * 0.007_kg * 0.002_m * 0.002_m,   0.1_sc * 0.007f,  0.20_sc)},
    {"Light",          DRIVER_LIGHT_PATH,          std::make_shared<Driver>(0.0015_m, 0.012_m, 0.01_m,  0.004_kg, 0.4f * 0.004_kg * 0.0015_m * 0.001_m,  0.08_sc * 0.004f, 0.18_sc)},
    {"High Friction",  DRIVER_HIGH_FRICTION_PATH,  std::make_shared<Driver>(0.002_m,  0.012_m, 0.015_m, 0.005_kg, 0.5f * 0.005_kg * 0.002_m * 0.0015_m,  0.1_sc * 0.005f,  0.35_sc)}
};