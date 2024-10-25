#pragma once

#include "Beyblade.h"
#include <string>
#include <vector>
#include <memory>

class Profile {
public:
    Profile(const std::string& name)
        : name(name) {}

    std::string getName() const { return name; }

    void addBeyblade(const std::string& name) {
        std::shared_ptr<Beyblade> beyblade = std::make_shared<Beyblade>(name);
        beybladesOwned.push_back(beyblade);
    }
    // Adds a Beyblade to the profile's list of owned Beyblades
    void addBeyblade(std::shared_ptr<Beyblade> beyblade) {
        beybladesOwned.push_back(beyblade);
    }

    bool deleteBeyblade(const std::string& name) {
        for (auto it = beybladesOwned.begin(); it != beybladesOwned.end(); ++it) {
            if ((*it)->getName() == name) {
                beybladesOwned.erase(it);
                return true;
            }
        }
        return false;
    }

    // Retrieves the list of owned Beyblades
    std::vector<std::shared_ptr<Beyblade>>& getBeybladesOwned() {
        return beybladesOwned;
    }

private:
    std::string name{};
    std::vector<std::shared_ptr<Beyblade>> beybladesOwned{}; // List of Beyblades owned by the profile
};
