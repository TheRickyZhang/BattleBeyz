#include "Profile.h"

// SERVER: (need globally unique beybladeIds across multiple different profiles)
bool Profile::createBeyblade(const std::string& name) {
    std::cout << "Attempting to create Beyblade: " << name << std::endl;
    std::lock_guard<std::mutex> lock(mtx);
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        std::cerr << "Error: Cannot add Beyblade. Maximum number of beyblades (" << MAX_BEYBLADES_PER_PROFILE << ") reached." << std::endl;
        return false;
    }

    nextBladeID++;
    if (!beybladesOwned.empty() && getBeybladeIterator(nextBladeID) != beybladesOwned.end()) {
        std::cerr << "Error: Beyblade with ID " << nextBladeID << " already exists." << std::endl;
        return false;
    }

    auto beyblade = std::make_shared<Beyblade>(nextBladeID, name);


    beybladesOwned.push_back(beyblade);
    if (!activeBeybladeId.has_value()) {
        activeBeybladeId = beyblade->getId();
    }
    std::cout << "Beyblade created successfully with ID: " << beyblade->getId() << std::endl;
    return true;
}


// Return false if beyblade already exists or max beyblades reached.
bool Profile::addBeyblade(std::shared_ptr<Beyblade> beyblade) {
    std::lock_guard<std::mutex> lock(mtx);
    if (getBeybladeIterator(beyblade->getId()) == beybladesOwned.end()) {
        return false;
    }
    if (beybladesOwned.size() >= MAX_BEYBLADES_PER_PROFILE) {
        return false;
    }
    beybladesOwned.push_back(beyblade);
    if (!activeBeybladeId.has_value()) {
        activeBeybladeId = beyblade->getId();
    }
    return true;
}


bool Profile::deleteBeyblade(int beybladeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    if (it == beybladesOwned.end()) {
        std::cerr << "Error: Beyblade with ID " << beybladeId << " not found in profile " << id << "." << std::endl;
        return false;
    }

    // Erase the Beyblade first to ensure safe access
    bool wasSelected = (activeBeybladeId && *activeBeybladeId == beybladeId);
    beybladesOwned.erase(it);

    if (wasSelected) {
        activeBeybladeId.reset();
        if (!beybladesOwned.empty()) {
            activeBeybladeId = beybladesOwned.front()->getId(); // Default to the first remaining Beyblade
        }
    }

    return true;
}

std::shared_ptr<Beyblade> Profile::getBeyblade(int beybladeId) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    for (auto bey : beybladesOwned) {
        std::cout << bey->getName() << "," << bey->getId() << " ";
    }
    std::cout << "|" << beybladeId << std::endl;
    if (it == beybladesOwned.end()) return nullptr;
    return *it;
}

// Take an optional id, or uses active one by default.
// Returns the beyblade pointer or nullptr if not found
std::shared_ptr<Beyblade> Profile::getActiveBeyblade() const {
    std::lock_guard<std::mutex> lock(mtx);
    if (!activeBeybladeId.has_value()) {
        return nullptr;
    }
    auto it = getBeybladeIterator(activeBeybladeId.value());
    if (it != beybladesOwned.end()) {
        return *it;
    }
    else {
        return nullptr;
    }
}

bool Profile::setActiveBeyblade(int beybladeId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = getBeybladeIterator(beybladeId);
    if (it != beybladesOwned.end()) {
        activeBeybladeId = beybladeId;
        return true;
    }
    return false;
}

const std::vector<std::shared_ptr<Beyblade>>& Profile::getAllBeyblades() const {
    std::lock_guard<std::mutex> lock(mtx);
    return beybladesOwned;
}