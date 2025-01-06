#pragma once

#include <string>
#include <vector>
#include <memory>

struct Layer;
struct Disc;
struct Driver;

template <typename PartType>
struct TemplateFormat {
    std::string name;
    std::string modelPath;
    std::shared_ptr<PartType> part;

    TemplateFormat(const std::string& name, const std::string& modelPath, std::shared_ptr<PartType> part)
        : name(name), modelPath(modelPath), part(part) {}
};

extern std::vector<TemplateFormat<Layer>> templateLayers;  // TODO: Make static to avoid stack overflow
extern std::vector<TemplateFormat<Disc>> templateDiscs;
extern std::vector<TemplateFormat<Driver>> templateDrivers;
