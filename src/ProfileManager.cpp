#include "ProfileManager.h"
#include "json.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace nlohmann;

// TEMP to start off with defaults
void ProfileManager::addDefaultProfiles() {
    if (!createProfile("Default") || !createProfile("Player1")) {
        cerr << "Error: Failed to create default profiles." << endl;
        return;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade1")) {
        cout << "Cannot create 1" << endl;
    }
    if (!getActiveProfile()->createBeyblade("DefaultBeyblade2")) {
        cout << "Cannot create 2" << endl;
    }
}


// SERVER: To replace with server requests for unique profileIds
bool ProfileManager::createProfile(const string& name) {
    lock_guard<mutex> lock(mtx);
    if (profiles.size() >= MAX_PROFILES) {
        cerr << "Error: Cannot add profile. Maximum number of profiles (" << MAX_PROFILES << ") reached." << endl;
        return false;
    }
    
    
    nextProfileID++;
    if (profiles.size() > 0 && getProfileIterator(nextProfileID) != profiles.end()) {
        return false;
    }
    auto profile = make_shared<Profile>(nextProfileID, name);
    profiles.push_back(profile);
    if (!activeProfileId.has_value()) {
        activeProfileId = profile->getId();  // Set newly added profile as active if none active
    }
    return true;
}

bool ProfileManager::addProfile(shared_ptr<Profile> profile) {
    lock_guard<mutex> lock(mtx);
    if (profiles.size() >= MAX_PROFILES) {
        return false;
    }
    if (getProfileIterator(profile->getId()) != profiles.end()) {
        return false;
    }
    profiles.push_back(profile);
    if (!activeProfileId.has_value()) {
        activeProfileId = profile->getId();
    }
    return true;
}

// Deletes a profile by ID
bool ProfileManager::deleteProfile(int profileId) {
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (profiles.size() <= 1 || it == profiles.end()) {
        // cerr << "Error: Profile with ID " << profileId << " not found." << endl;
        return false;
    }

    bool wasSelected = (activeProfileId && *activeProfileId == profileId);
    profiles.erase(it);
    if (wasSelected) {
        if (!profiles.empty()) {
            activeProfileId = profiles.front()->getId(); // Default to the first remaining profile
        }
        else {
            activeProfileId.reset();
        }
    }
    return true;
}

shared_ptr<Profile> ProfileManager::getProfile(int profileId) const {
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Nullptr if no active profile
shared_ptr<Profile> ProfileManager::getActiveProfile() const {
    lock_guard<mutex> lock(mtx);
    if (!activeProfileId.has_value()) {
        return nullptr;
    }
    auto it = getProfileIterator(activeProfileId.value());
    if (it == profiles.end()) return nullptr;
    return *it;
}

// Sets a profile as the active profile by ID
bool ProfileManager::setActiveProfile(int profileId) {
    lock_guard<mutex> lock(mtx);
    auto it = getProfileIterator(profileId);
    if (it != profiles.end()) {
        activeProfileId = profileId;
        return true;
    }
    return false;
}

const vector<shared_ptr<Profile>>& ProfileManager::getAllProfiles() const {
    lock_guard<mutex> lock(mtx);
    return profiles;
}

void ProfileManager::saveProfilesToFile(const std::string& filePath) {
    json js;
    for (const auto& profile : profiles) {
        js["profiles"].push_back(profile->toJson()); // Save all relevant profile details
    }
    if (activeProfileId.has_value()) {
        js["activeProfileId"] = activeProfileId.value();
    }
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << js.dump(4); // Pretty print with 4 spaces
    }
}

void ProfileManager::loadProfilesFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        json js;
        file >> js;

        for (const auto& profileJson : js["profiles"]) {
            // Extract profile details
            std::string name = profileJson["name"];
            int id = profileJson["id"];

            // Use `addProfile` to add a pre-created profile to ensure proper initialization
            auto profile = std::make_shared<Profile>(id, name);
            profile->fromJson(profileJson); // Populate the profile's details

            // Add the profile via your addProfile function
            if (!addProfile(profile)) {
                std::cerr << "Error: Failed to add profile during load. Name: " << name << std::endl;
            }
        }

        // Set the active profile (optional: based on saved state)
        if (js.contains("activeProfileId")) {
            setActiveProfile(js["activeProfileId"]);
        }
    }
}