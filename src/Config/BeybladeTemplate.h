#pragma once

#include <string>
#include <vector>

struct Layer;
struct Disc;
struct Driver;

template <typename PartType>
struct TemplateFormat {
    std::string name;
    std::string modelPath;
    PartType part;

    TemplateFormat(const std::string& name, const std::string& modelPath, const PartType& part)
        : name(name), modelPath(modelPath), part(part) {}
};

extern std::vector<TemplateFormat<Layer>> templateLayers;
extern std::vector<TemplateFormat<Disc>> templateDiscs;
extern std::vector<TemplateFormat<Driver>> templateDrivers;
